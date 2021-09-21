#include "CAEN_FELib.h"
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
#include "germain.hh"
#include "attila.hh"
#include "sibilla.hh"
#include <iostream>

namespace {
  const std::string caen_error (int err) {
    static const std::map<int, std::string> errors{{CAEN_FELib_Success,"CAEN_FELib_Success"},
      {CAEN_FELib_GenericError,"CAEN_FELib_GenericError"},
      {CAEN_FELib_InvalidParam,"CAEN_FELib_InvalidParam"},
      {CAEN_FELib_DeviceAlreadyOpen,"CAEN_FELib_DeviceAlreadyOpen"},
      {CAEN_FELib_DeviceNotFound,"CAEN_FELib_DeviceNotFound"},
      {CAEN_FELib_MaxDevicesError,"CAEN_FELib_MaxDevicesError"},
      {CAEN_FELib_CommandError,"CAEN_FELib_CommandError"},
      {CAEN_FELib_InternalError,"CAEN_FELib_InternalError"},
      {CAEN_FELib_NotImplemented,"CAEN_FELib_NotImplemented"},
      {CAEN_FELib_InvalidHandle,"CAEN_FELib_InvalidHandle"},
      {CAEN_FELib_DeviceLibraryNotAvailable,"CAEN_FELib_DeviceLibraryNotAvailable"},
      {CAEN_FELib_Timeout,"CAEN_FELib_Timeout"},
      {CAEN_FELib_Stop,"CAEN_FELib_Stop"},
      {CAEN_FELib_Disabled,"CAEN_FELib_Disabled"},
      {CAEN_FELib_BadLibraryVersion,"CAEN_FELib_BadLibraryVersion"}};
    auto it = errors.find (err);
    if (it == errors.end ()) return "CAEN_Unknown_error";
    return it->second + "\n";
  }
}

germain::germain() {
}

germain::~germain() {
  close_link ();
}

void germain::init () {
  init_link ();

  init_channels ();

  init_trigger ();

  init_buffers ();

  init_metadata ();
}

void germain::close_link () {
  int ret = CAEN_FELib_Close(handle_);
  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_DGTZ_CloseDigitizer(" << handle_ << "): " << caen_error (ret);
}


void germain::init_link () {
  int ret = CAEN_FELib_Open(sibilla::evoke ()["path"].as<std::string>().c_str(), &handle_);

  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_Open(" << sibilla::evoke ()["path"].as<std::string>() << "):" << caen_error (ret);
  usleep (100000);
}

#if 0
void germain::init_channels () {
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SetDESMode (handle_, sibilla::evoke ()("des-mode") ?  CAEN_DGTZ_ENABLE : CAEN_DGTZ_DISABLE);
  if (err != CAEN_DGTZ_Success && (err != CAEN_DGTZ_FunctionNotAllowed || sibilla::evoke ()("des-mode"))) ATTILA << " CAEN_DGTZ_SetDESMode(" << handle_ << ",true): " << caen_error (err);

  set_register (sibilla::evoke ()("test-pattern") ? 0x8004 : 0x8008, 1 << 3);
  set_register (sibilla::evoke ()("overlap-trigger") ? 0x8004 : 0x8008, 1 << 1);

  bool positive_pulse = sibilla::evoke ()("positive-pulse");
  std::vector<int> channels = sibilla::evoke ()["channel-id"].as<std::vector<int>>();
  std::vector<int> channel_thresholds = sibilla::evoke ()["channel-threshold"].as<std::vector<int>>();
  std::vector<int> dc_offsets = sibilla::evoke ()["dc-offset"].as<std::vector<int>>();
  if (channel_thresholds.size () == 1) for (int i = 1; i < channels.size (); i++) channel_thresholds.push_back(channel_thresholds[0]);
  if (channel_thresholds.size () != channels.size ()) ATTILA << "channels.size != channel_thresholds.size";
  if (dc_offsets.size () == 1) for (int i = 1; i < channels.size (); i++) dc_offsets.push_back(dc_offsets[0]);
  if (dc_offsets.size () != channels.size ()) ATTILA << "channels.size != dc_offsets.size";

  uint16_t channels_mask = 0;
  bool enable_channel_threshold = false;

  for (int i = 0; i < channels.size(); i++) {
    int ch = channels[i];
    int channel_threshold = channel_thresholds[i];
    int dc_offset = dc_offsets[i];

    channels_mask |= 1 << ch;

    err = CAEN_DGTZ_SetChannelDCOffset (handle_, ch, dc_offset);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetChannelDCOffset(" << handle_ << "," << ch << "," << dc_offset << "): " << caen_error (err);

    if (channel_threshold < 0) continue;

    err = CAEN_DGTZ_SetChannelTriggerThreshold(handle_, ch, channel_threshold);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetChannelTriggerThreshold(" << handle_ << "," << ch << "," << channel_threshold << "): " << caen_error (err);

    err = CAEN_DGTZ_SetTriggerPolarity(handle_, ch, positive_pulse ? CAEN_DGTZ_TriggerOnRisingEdge : CAEN_DGTZ_TriggerOnFallingEdge);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetTriggerPolarity(" << handle_ << "," << ch << "," << (positive_pulse ? CAEN_DGTZ_TriggerOnRisingEdge : CAEN_DGTZ_TriggerOnFallingEdge) << "): " << caen_error (err);

    enable_channel_threshold = true;
  }

  err = CAEN_DGTZ_SetChannelEnableMask (handle_, channels_mask);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetChannelEnableMask(" << handle_ << "," << channels_mask << "): " << caen_error (err);

  err = CAEN_DGTZ_SetChannelSelfTrigger (handle_, !enable_channel_threshold ? CAEN_DGTZ_TRGMODE_DISABLED : CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT, channels_mask);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetChannelSelfTrigger(" << handle_ << (!enable_channel_threshold ? "CAEN_DGTZ_TRGMODE_DISABLED" : "CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT") << "," << channels_mask << "): " << caen_error (err);

  int majority = sibilla::evoke ()["majority"].as<int>();
  if (majority < 1) ATTILA << " majority has to be between 1 (no majority) and # channels";
  else if (majority > 1) set_register_bits(0x810C, (uint32_t(majority - 1) << 24) | (0xF << 20));
//  else clear_register_bits(0x810C, 0x7 << 24); // default
}

void germain::init_trigger () {
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SetExtTriggerInputMode (handle_, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetExtTriggerInputMode(" << handle_ << ",CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT): " << caen_error (err);

  err = CAEN_DGTZ_SetSWTriggerMode(handle_, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetSWTriggerMode(" << handle_ << ",CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT): " << caen_error (err);

  err = CAEN_DGTZ_SetIOLevel (handle_, sibilla::evoke ()("nim") ? CAEN_DGTZ_IOLevel_NIM : CAEN_DGTZ_IOLevel_TTL);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetIOLevel(" << handle_ << "," << (sibilla::evoke ()("nim") ? CAEN_DGTZ_IOLevel_NIM : CAEN_DGTZ_IOLevel_TTL) << "): " << caen_error (err);

  uint32_t record_length = sibilla::evoke ()["gate-width"].as<int>();
  err = CAEN_DGTZ_SetRecordLength (handle_, record_length);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetRecordLength(" << handle_ << "," << record_length << "): " << caen_error (err);

  int post_trigger = sibilla::evoke ()["post-trigger"].as<int>();
  err = CAEN_DGTZ_SetPostTriggerSize (handle_, post_trigger);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetPostTriggerSize(" << handle_ << "," << post_trigger << "): " << caen_error (err);

  err = CAEN_DGTZ_SetAcquisitionMode (handle_, CAEN_DGTZ_SW_CONTROLLED);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetAcquisitionMode(" << handle_ << "," << CAEN_DGTZ_SW_CONTROLLED << "): " << caen_error (err);

  sw_trigger_ = sibilla::evoke ()("software-trigger");
}

void germain::init_buffers () {
  //if (int(get_register(0x800C)) > max_buffers_code) set_register (0x800C, max_buffers_code);

  int max_events_blt = 20;
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SetMaxNumEventsBLT (handle_, max_events_blt);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetMaxNumEventsBLT(" << handle_ << ","<< max_events_blt << "): " << caen_error (err);

  err = CAEN_DGTZ_SetInterruptConfig (handle_, CAEN_DGTZ_ENABLE, 1, 1, 1, CAEN_DGTZ_IRQ_MODE_RORA);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SetInterruptConfig(" << handle_ << ",CAEN_DGTZ_ENABLE,1,1,CAEN_DGTZ_IRQ_MODE_RORA): " << caen_error (err);

  err = CAEN_DGTZ_MallocReadoutBuffer (handle_, &event_buffer_, &buffer_size_);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_MallocReadoutBuffer(" << handle_ << "): " << caen_error (err);
}

void germain::init_metadata () {
  CAEN_DGTZ_BoardInfo_t BoardInfo;
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_GetInfo (handle_, &BoardInfo);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_GetInfo(" << handle_ << "): " << caen_error (err);

  std::cout << "found digitizer " << BoardInfo.SerialNumber << " model " << BoardInfo.ModelName << " ROC # " << BoardInfo.ROC_FirmwareRel << " AMC # " << BoardInfo.AMC_FirmwareRel << std::endl;

  aristotele_.board = 1700 + v17xx_modules[(CAEN_DGTZ_BoardFamilyCode_t(BoardInfo.FamilyCode))];
  aristotele_.n_bits = BoardInfo.ADC_NBits;
  aristotele_.sampling_rate = sample_rates_MHz[CAEN_DGTZ_BoardFamilyCode_t(BoardInfo.FamilyCode)] * (1 + 1 * sibilla::evoke ()("des-mode"));
  aristotele_.gate_length = sibilla::evoke ()["gate-width"].as<int>();
  aristotele_.post_trigger = sibilla::evoke ()["post-trigger"].as<int>();
  aristotele_.threshold = sibilla::evoke ()["channel-threshold"].as<std::vector<int>>()[0];
}

void germain::start () {
  if (!emulate_hw_) {
    CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SWStartAcquisition (handle_);
    if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SWStartAcquisition(" << handle_ << "): " << caen_error (err);
    usleep (10000);
  }

  start_time_ = std::chrono::system_clock::now ();
}

void germain::stop () {
  if (emulate_hw_) return;

  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SWStopAcquisition (handle_);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_SWStopAcquisition(" << handle_ << "): " << caen_error (err);
}

std::vector<std::unique_ptr<evaristo>> germain::loop() {
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
      if (aristotele_.n_bits > 8) {
	CAEN_DGTZ_UINT16_EVENT_t *decoded_event = reinterpret_cast<CAEN_DGTZ_UINT16_EVENT_t*>(decoded_event_);

	int total_size = sizeof(evaristo)/sizeof(uint16_t);
	for (int k = 0; k < MAX_UINT16_CHANNEL_SIZE; k++) { 
	  total_size += decoded_event->ChSize[k];
          if (decoded_event->ChSize[k] > 0) n_samples = decoded_event->ChSize[k];
	}
	
	ev = std::unique_ptr<evaristo>(reinterpret_cast<evaristo*>(new uint16_t[total_size]));

	uint16_t *ptr = ev->samples;
	for (int k = 0; k < MAX_UINT16_CHANNEL_SIZE; k++) 
	  if (decoded_event->ChSize[k] == n_samples) { 
            memcpy (ptr, decoded_event->DataChannel[k], n_samples * sizeof(uint16_t));
	    ptr += n_samples;
            n_channels ++;
	  }
      } else {
	CAEN_DGTZ_UINT8_EVENT_t *decoded_event = reinterpret_cast<CAEN_DGTZ_UINT8_EVENT_t*>(decoded_event_);

	int total_size = sizeof(evaristo)/sizeof(uint16_t);
	for (int k = 0; k < MAX_UINT8_CHANNEL_SIZE; k++) { 
	  total_size += decoded_event->ChSize[k];
          if (decoded_event->ChSize[k] > 0) n_samples = decoded_event->ChSize[k];
	}
	
	ev = std::unique_ptr<evaristo>(reinterpret_cast<evaristo*>(new uint16_t[total_size]));

	uint16_t *ptr = ev->samples;
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

void germain::set_register (uint16_t reg, uint32_t val) {
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_WriteRegister (handle_, reg, val);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_WriteRegister(" << handle_ << "," << std::hex << std::showbase << reg << "," << val << "): " << caen_error (err);
}

uint32_t germain::get_register (uint16_t reg) {
  uint32_t val;

  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_ReadRegister (handle_, reg, &val);
  if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_ReadRegister(" << handle_ << "," << std::hex << std::showbase << reg << "): " << caen_error (err);

  return val;
}

uint32_t germain::set_register_bits (uint16_t reg, uint32_t bits) {
  uint32_t val = get_register (reg);

  if (bits) {
    val |= bits;
    set_register (reg, val);
  }

  return val;
}

uint32_t germain::clear_register_bits (uint16_t reg, uint32_t bits) {
  uint32_t val = get_register (reg);

  if (bits) {
    val &= ~bits;
    set_register (reg, val);
  }

  return val;
}

bool germain::wait_irq () {
  if (!wait_irq_) {
    usleep (1000);
    return false;
  }
  CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_IRQWait (handle_, 10);
  if (err == CAEN_DGTZ_Timeout) return false;
  else if (err == CAEN_DGTZ_GenericError) {
    BRENNO << " ignoring CAEN_DGTZ_GenericError in CAEN_DGTZ_IRQWait";
    return false;
  }
  else if (err != CAEN_DGTZ_Success) ATTILA << " CAEN_DGTZ_IRQWait(" << handle_ << "," << 100 << "): " << caen_error (err);

  return true;
}

std::vector<std::unique_ptr<evaristo>> germain::emulate_loop () {
  std::vector<std::unique_ptr<evaristo>> ev_v;
  static int c = 0;

  int n = sibilla::evoke ()["gate-width"].as<int>();

  std::unique_ptr<evaristo> ev = std::unique_ptr<evaristo>(reinterpret_cast<evaristo*>(new uint16_t[sizeof(evaristo)/2 + n]));
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
#endif
