#include "CAENDigitizer.h"
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <string.h>
#include <cmath>
#include <stdlib.h>
#include "flamel.hh"
#include "attila.hh"
#include "sibilla.hh"
#include <iostream>

namespace {
  const std::string caen_error (CAEN_DGTZ_ErrorCode err) {
    static const std::map<CAEN_DGTZ_ErrorCode, std::string> errors{{CAEN_DGTZ_Success, "CAEN_DGTZ_Success" },
            { CAEN_DGTZ_CommError, "CAEN_DGTZ_CommError" },
            { CAEN_DGTZ_GenericError, "CAEN_DGTZ_GenericError" },
            { CAEN_DGTZ_InvalidParam, "CAEN_DGTZ_InvalidParam" },
            { CAEN_DGTZ_InvalidLinkType, "CAEN_DGTZ_InvalidLinkType" },
          //  { CAEN_DGTZ_InvalidHandler, "CAEN_DGTZ_InvalidHandler" },
            { CAEN_DGTZ_MaxDevicesError, "CAEN_DGTZ_MaxDevicesError" },
            { CAEN_DGTZ_BadBoardType, "CAEN_DGTZ_BadBoardType" },
            { CAEN_DGTZ_BadInterruptLev, "CAEN_DGTZ_BadInterruptLev" },
            { CAEN_DGTZ_BadEventNumber, "CAEN_DGTZ_BadEventNumber" },
            { CAEN_DGTZ_ReadDeviceRegisterFail, "CAEN_DGTZ_ReadDeviceRegisterFail" },
            { CAEN_DGTZ_WriteDeviceRegisterFail, "CAEN_DGTZ_WriteDeviceRegisterFail" },
            { CAEN_DGTZ_InvalidChannelNumber, "CAEN_DGTZ_InvalidChannelNumber" },
            { CAEN_DGTZ_ChannelBusy, "CAEN_DGTZ_ChannelBusy" },
            { CAEN_DGTZ_FPIOModeInvalid, "CAEN_DGTZ_FPIOModeInvalid" },
            { CAEN_DGTZ_WrongAcqMode, "CAEN_DGTZ_WrongAcqMode" },
            { CAEN_DGTZ_FunctionNotAllowed, "CAEN_DGTZ_FunctionNotAllowed" },
            { CAEN_DGTZ_Timeout, "CAEN_DGTZ_Timeout" },
            { CAEN_DGTZ_InvalidBuffer, "CAEN_DGTZ_InvalidBuffer" },
            { CAEN_DGTZ_EventNotFound, "CAEN_DGTZ_EventNotFound" },
            { CAEN_DGTZ_InvalidEvent, "CAEN_DGTZ_InvalidEvent" },
            { CAEN_DGTZ_OutOfMemory, "CAEN_DGTZ_OutOfMemory" },
            { CAEN_DGTZ_CalibrationError, "CAEN_DGTZ_CalibrationError" },
            { CAEN_DGTZ_DigitizerNotFound, "CAEN_DGTZ_DigitizerNotFound" },
            { CAEN_DGTZ_DigitizerAlreadyOpen, "CAEN_DGTZ_DigitizerAlreadyOpen" },
            { CAEN_DGTZ_DigitizerNotReady, "CAEN_DGTZ_DigitizerNotReady" },
            { CAEN_DGTZ_InterruptNotConfigured, "CAEN_DGTZ_InterruptNotConfigured" },
            { CAEN_DGTZ_DigitizerMemoryCorrupted, "CAEN_DGTZ_DigitizerMemoryCorrupted" },
            { CAEN_DGTZ_DPPFirmwareNotSupported, "CAEN_DGTZ_DPPFirmwareNotSupported" },
            { CAEN_DGTZ_NotYetImplemented, "CAEN_DGTZ_NotYetImplemented" }};
    auto it = errors.find (err);
    if (it == errors.end ()) return "CAEN_Unknown_error";
    return it->second + "\n";
  }
  static std::map<CAEN_DGTZ_BoardFamilyCode_t, int> sample_rates_MHz{{ CAEN_DGTZ_XX724_FAMILY_CODE, 100 },
            { CAEN_DGTZ_XX721_FAMILY_CODE, 500 },
            { CAEN_DGTZ_XX731_FAMILY_CODE, 500 },
            { CAEN_DGTZ_XX720_FAMILY_CODE, 250 },
            { CAEN_DGTZ_XX740_FAMILY_CODE, 65 },
            { CAEN_DGTZ_XX751_FAMILY_CODE, 1000 },
            { CAEN_DGTZ_XX742_FAMILY_CODE, 5000 },
            { CAEN_DGTZ_XX780_FAMILY_CODE, 100 },
            { CAEN_DGTZ_XX761_FAMILY_CODE, 4000 }};

  static std::map<CAEN_DGTZ_BoardFamilyCode_t, int> v17xx_modules{{ CAEN_DGTZ_XX724_FAMILY_CODE, 24 },
            { CAEN_DGTZ_XX721_FAMILY_CODE, 21 },
            { CAEN_DGTZ_XX731_FAMILY_CODE, 31 },
            { CAEN_DGTZ_XX720_FAMILY_CODE, 20 },
            { CAEN_DGTZ_XX740_FAMILY_CODE, 40 },
            { CAEN_DGTZ_XX751_FAMILY_CODE, 51 },
            { CAEN_DGTZ_XX742_FAMILY_CODE, 42 },
            { CAEN_DGTZ_XX780_FAMILY_CODE, 80 },
            { CAEN_DGTZ_XX761_FAMILY_CODE, 61 }};
}

flamel::flamel(): decoded_event_(0) { 
  emulate_hw_ = sibilla::evoke ()("emulate-hw");
}

flamel::~flamel() {
  if (emulate_hw_) return;

  CAEN_DGTZ_FreeEvent (handle_, &decoded_event_); 

  close_link ();
}

void flamel::init () {
  if (emulate_hw_) {
    metadata_.board = -1751;
    metadata_.n_bits = 10;
    metadata_.sampling_rate = 1000000000;
    return;
  }

  init_link ();

  init_channels ();

  init_trigger ();

  init_buffers ();

  init_metadata ();
}

void flamel::close_link () {
  CAEN_DGTZ_FreeReadoutBuffer (&event_buffer_);

  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_CloseDigitizer (handle_);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_CloseDigitizer(" << handle_ << "): " << caen_error (err);
}


void flamel::init_link () {
  CAEN_DGTZ_ConnectionType link_type = sibilla::evoke ()("usb-link") ? CAEN_DGTZ_USB : CAEN_DGTZ_PCI_OpticalLink;
  int link_id = sibilla::evoke ()["link-number"].as<int>();
  int node_id = sibilla::evoke ()["node-number"].as<int>();
  u_int32_t vme_base = sibilla::evoke ()["vme-base"].as<u_int32_t>();
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_OpenDigitizer (link_type, link_id, node_id, vme_base, &handle_);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_OpenDigitizer(" << link_type << "," << link_id << "," << node_id << "," << vme_base << "): " << caen_error (err);

  err = CAEN_DGTZ_Reset (handle_);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_Reset(" << handle_ << "):" << caen_error (err);
  usleep (100000);
}

void flamel::init_channels () {
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SetDESMode (handle_, sibilla::evoke ()("des-mode") ?  CAEN_DGTZ_ENABLE : CAEN_DGTZ_DISABLE);
  if (err != CAEN_DGTZ_Success && (err != CAEN_DGTZ_FunctionNotAllowed || sibilla::evoke ()("des-mode"))) ATTILA << " CAEN_DGTZ_SetDESMode(" << handle_ << ",true): " << caen_error (err);

  set_register (sibilla::evoke ()("test-pattern") ? 0x8004 : 0x8008, 1 << 3);
  set_register (sibilla::evoke ()("overlap-trigger") ? 0x8004 : 0x8008, 1 << 1);

  int dc_offset = sibilla::evoke ()["dc-offset"].as<int>();
  int channel_threshold = sibilla::evoke ()["channel-threshold"].as<int>();
  bool positive_pulse = sibilla::evoke ()("positive-pulse");
  std::vector<int> channels = sibilla::evoke ()["channel-id"].as<std::vector<int>>();
  u_int8_t channels_mask = 0;
  for (int ch: channels) {
    channels_mask |= 1 << ch;

    err = CAEN_DGTZ_SetChannelDCOffset (handle_, ch, dc_offset);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetChannelDCOffset(" << handle_ << "," << ch << "," << dc_offset << "): " << caen_error (err);

    if (channel_threshold < 0) continue;

    err = CAEN_DGTZ_SetChannelTriggerThreshold(handle_, ch, channel_threshold);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetChannelTriggerThreshold(" << handle_ << "," << ch << "," << channel_threshold << "): " << caen_error (err);

    err = CAEN_DGTZ_SetTriggerPolarity(handle_, ch, positive_pulse ? CAEN_DGTZ_TriggerOnRisingEdge : CAEN_DGTZ_TriggerOnFallingEdge);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetTriggerPolarity(" << handle_ << "," << ch << "," << (positive_pulse ? CAEN_DGTZ_TriggerOnRisingEdge : CAEN_DGTZ_TriggerOnFallingEdge) << "): " << caen_error (err);
  }

  err = CAEN_DGTZ_SetChannelEnableMask (handle_, channels_mask);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetChannelEnableMask(" << handle_ << "," << channels_mask << "): " << caen_error (err);

  err = CAEN_DGTZ_SetChannelSelfTrigger (handle_, (channel_threshold < 0) ? CAEN_DGTZ_TRGMODE_DISABLED : CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT, channels_mask);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetChannelSelfTrigger(" << handle_ << ((channel_threshold < 0) ? "CAEN_DGTZ_TRGMODE_DISABLED" : "CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT") << "," << channels_mask << "): " << caen_error (err);

}

void flamel::init_trigger () {
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SetExtTriggerInputMode (handle_, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetExtTriggerInputMode(" << handle_ << ",CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT): " << caen_error (err);

  err = CAEN_DGTZ_SetSWTriggerMode(handle_, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetSWTriggerMode(" << handle_ << ",CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT): " << caen_error (err);

  err = CAEN_DGTZ_SetIOLevel (handle_, sibilla::evoke ()("nim") ? CAEN_DGTZ_IOLevel_NIM : CAEN_DGTZ_IOLevel_TTL);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetIOLevel(" << handle_ << "," << (sibilla::evoke ()("nim") ? CAEN_DGTZ_IOLevel_NIM : CAEN_DGTZ_IOLevel_TTL) << "): " << caen_error (err);

  u_int32_t record_length = sibilla::evoke ()["gate-width"].as<int>();
  err = CAEN_DGTZ_SetRecordLength (handle_, record_length);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetRecordLength(" << handle_ << "," << record_length << "): " << caen_error (err);

  int post_trigger = sibilla::evoke ()["post-trigger"].as<int>();
  err = CAEN_DGTZ_SetPostTriggerSize (handle_, post_trigger);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetPostTriggerSize(" << handle_ << "," << post_trigger << "): " << caen_error (err);

  err = CAEN_DGTZ_SetAcquisitionMode (handle_, CAEN_DGTZ_SW_CONTROLLED);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetAcquisitionMode(" << handle_ << "," << CAEN_DGTZ_SW_CONTROLLED << "): " << caen_error (err);

  sw_trigger_ = sibilla::evoke ()("software-trigger");
}

void flamel::init_buffers () {
  //if (int(get_register(0x800C)) > max_buffers_code) set_register (0x800C, max_buffers_code);

  int max_events_blt = 20;
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SetMaxNumEventsBLT (handle_, max_events_blt);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetMaxNumEventsBLT(" << handle_ << ","<< max_events_blt << "): " << caen_error (err);

  err = CAEN_DGTZ_SetInterruptConfig (handle_, CAEN_DGTZ_ENABLE, 1, 1, 1, CAEN_DGTZ_IRQ_MODE_RORA);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetInterruptConfig(" << handle_ << ",CAEN_DGTZ_ENABLE,1,1,CAEN_DGTZ_IRQ_MODE_RORA): " << caen_error (err);

  err = CAEN_DGTZ_MallocReadoutBuffer (handle_, &event_buffer_, &buffer_size_);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_MallocReadoutBuffer(" << handle_ << "): " << caen_error (err);
}

void flamel::init_metadata () {
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_GetInfo (handle_, &BoardInfo);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_GetInfo(" << handle_ << "): " << caen_error (err);

  std::cout << "found digitizer " << BoardInfo.SerialNumber << " model " << BoardInfo.ModelName << " ROC # " << BoardInfo.ROC_FirmwareRel << " AMC # " << BoardInfo.AMC_FirmwareRel << std::endl;

  metadata_.board = 1700 + v17xx_modules[(CAEN_DGTZ_BoardFamilyCode_t(BoardInfo.FamilyCode))];
  metadata_.n_bits = BoardInfo.ADC_NBits;
  metadata_.sampling_rate = sample_rates_MHz[CAEN_DGTZ_BoardFamilyCode_t(BoardInfo.FamilyCode)] * (1 + 1 * sibilla::evoke ()("des-mode"));
}

void flamel::start () {
  if (!emulate_hw_) {
    CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SWStartAcquisition (handle_);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SWStartAcquisition(" << handle_ << "): " << caen_error (err);
    usleep (10000);
  }

  start_time_ = std::chrono::system_clock::now ();
}

void flamel::stop () {
  if (emulate_hw_) return;

  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SWStopAcquisition (handle_);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SWStopAcquisition(" << handle_ << "): " << caen_error (err);
}

std::vector<std::unique_ptr<evaristo>> flamel::loop() {
  if (emulate_hw_) return emulate_loop ();

  std::vector<std::unique_ptr<evaristo>> ev_v;
  CAEN_DGTZ_ErrorCode err;

  if (sw_trigger_) {
    err = CAEN_DGTZ_SendSWtrigger (handle_);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SendSWtrigger(" << handle_ << "): " << caen_error (err);
  }

  for (int j = 0; j < 5; j++) {
    bool no_irq = (!(get_register (0x8104) & (1 << 3)) && !wait_irq ());
    if (no_irq) continue;

    uint32_t gross_size;
    err = CAEN_DGTZ_ReadData (handle_, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, event_buffer_, &gross_size);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_ReadData(" << handle_ << ",CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT," << event_buffer_ << "," << gross_size << "): " << caen_error (err);

    uint32_t n_events;
    err = CAEN_DGTZ_GetNumEvents (handle_, event_buffer_, gross_size, &n_events);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_GetNumEvents(" << handle_ << "," << event_buffer_ << "," << gross_size << "," << n_events << "): " << caen_error (err);

    for (uint32_t i = 0; i < n_events; i++) {
      CAEN_DGTZ_EventInfo_t event_info;
      char *event_ptr;
      err = CAEN_DGTZ_GetEventInfo (handle_, event_buffer_, gross_size, i, &event_info, &event_ptr);
      if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_GetEventInfo(" << handle_ << "," << event_buffer_ << "," << gross_size << "," << i << "," << event_info.EventCounter << "," << event_ptr << "): " << caen_error (err);

      CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_DecodeEvent (handle_, event_ptr, &decoded_event_);
      if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_DecodeEvent(" << handle_ << "," << event_ptr << ", NULL): " << caen_error (err); 

      std::unique_ptr<evaristo> ev;
      int n_samples = 0;
      int n_channels = 0;
      if (metadata_.n_bits > 8) {
	CAEN_DGTZ_UINT16_EVENT_t *decoded_event = reinterpret_cast<CAEN_DGTZ_UINT16_EVENT_t*>(decoded_event_);

	int total_size = sizeof(evaristo)/sizeof(u_int16_t);
	for (int k = 0; k < MAX_UINT16_CHANNEL_SIZE; k++) { 
	  total_size += decoded_event->ChSize[k];
          if (decoded_event->ChSize[k] > 0) n_samples = decoded_event->ChSize[k];
	}
	
	ev = std::unique_ptr<evaristo>(reinterpret_cast<evaristo*>(new u_int16_t[total_size]));

	u_int16_t *ptr = ev->samples;
	for (int k = 0; k < MAX_UINT16_CHANNEL_SIZE; k++) 
	  if (decoded_event->ChSize[k] == n_samples) { 
            memcpy (ptr, decoded_event->DataChannel[k], n_samples * sizeof(u_int16_t));
	    ptr += n_samples;
            n_channels ++;
	  }
      } else {
	CAEN_DGTZ_UINT8_EVENT_t *decoded_event = reinterpret_cast<CAEN_DGTZ_UINT8_EVENT_t*>(decoded_event_);

	int total_size = sizeof(evaristo)/sizeof(u_int16_t);
	for (int k = 0; k < MAX_UINT8_CHANNEL_SIZE; k++) { 
	  total_size += decoded_event->ChSize[k];
          if (decoded_event->ChSize[k] > 0) n_samples = decoded_event->ChSize[k];
	}
	
	ev = std::unique_ptr<evaristo>(reinterpret_cast<evaristo*>(new u_int16_t[total_size]));

	u_int16_t *ptr = ev->samples;
	for (int k = 0; k < MAX_UINT8_CHANNEL_SIZE; k++) 
	  if (decoded_event->ChSize[k] == n_samples) {
            for (int z = 0; z < n_samples; z++) ptr[z] = decoded_event->DataChannel[k][z];
	    ptr += n_samples;
            n_channels ++;
	  }
      }
      
      ev->n_samples = n_samples;
      ev->n_channels = n_channels;
      ev->time_tag = event_info.TriggerTimeTag;
      ev->counter = event_info.EventCounter;
      ev->unused[0] = ev->unused[1] = ev->unused[2] = ev->unused[3] = 0;
      ev->cpu_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now () - start_time_).count ();

      ev_v.push_back (std::move(ev));
    }
  }
  return ev_v;
}

void flamel::set_register (uint16_t reg, uint32_t val) {
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_WriteRegister (handle_, reg, val);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_WriteRegister(" << handle_ << "," << std::hex << std::showbase << reg << "," << val << "): " << caen_error (err);
}

uint32_t flamel::get_register (uint16_t reg) {
  uint32_t val;

  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_ReadRegister (handle_, reg, &val);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_ReadRegister(" << handle_ << "," << std::hex << std::showbase << reg << "): " << caen_error (err);

  return val;
}

uint32_t flamel::set_register_bits (uint16_t reg, uint32_t bits) {
  uint32_t val = get_register (reg);

  if (bits) {
    val |= bits;
    set_register (reg, val);
  }

  return val;
}

bool flamel::wait_irq () {
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_IRQWait (handle_, 10);
  if (err == CAEN_DGTZ_Timeout) return false;
  else if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_IRQWait(" << handle_ << "," << 100 << "): " << caen_error (err);

  return true;
}

std::vector<std::unique_ptr<evaristo>> flamel::emulate_loop () {
  std::vector<std::unique_ptr<evaristo>> ev_v;
  static int c = 0;

  int n = sibilla::evoke ()["gate-width"].as<int>();

  std::unique_ptr<evaristo> ev = std::unique_ptr<evaristo>(reinterpret_cast<evaristo*>(new u_int16_t[sizeof(evaristo)/2 + n]));
  ev->n_samples = n;
  ev->time_tag = ++c * 100;
  ev->counter = c;
  ev->cpu_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now () - start_time_).count ();

  int f = rand ();
  for (int i = 0; i < n; i++) ev->samples[i] = 512 + 400 * sin (2 * 3.14 * i / 500 + f);

  ev_v.push_back (std::move(ev));
  
  usleep (10000);
  return ev_v;
}
