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
#include <boost/lexical_cast.hpp>

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

template<> void germain::set (const std::string& path, const char* value) { _set(path, std::string(value)); }

germain::germain(): buffer_(0), sizes_(0) {
}

germain::~germain() {
  close_link ();

  if (buffer_) {
    for (unsigned int i = 0; i < board_channels_; ++i) delete[] buffer_[i];
    delete[] buffer_;
  }
  delete[] sizes_;
}

void germain::init () {
  init_link ();

  init_channels ();

  init_trigger ();

  init_buffers ();

  init_metadata ();
}

void germain::start () {
  send("/cmd/armacquisition");
  send("/cmd/swstartacquisition");

  start_time_ = std::chrono::system_clock::now ();
}

void germain::stop () {
  send("/cmd/disarmacquisition");
}


void germain::close_link () {
  int ret = CAEN_FELib_Close(handle_);
  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_DGTZ_CloseDigitizer(" << handle_ << "): " << caen_error (ret);
}


void germain::init_link () {
  std::string host = sibilla::evoke ()["host"].as<std::string>();
  if (host.empty()) ATTILA << "No host specified for CAEN_FELib_Open";
  int ret = CAEN_FELib_Open(("dig2:" + sibilla::evoke ()["host"].as<std::string>()).c_str(), &handle_);

  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_Open(dig2:" << sibilla::evoke ()["host"].as<std::string>() << "): " << caen_error (ret);
  usleep (100000);
}

void germain::init_channels () {
  bool positive_pulse = sibilla::evoke ()("positive-pulse");
  std::vector<int> channels = sibilla::evoke ()["channel-id"].as<std::vector<int>>();
  std::vector<int> channel_thresholds = sibilla::evoke ()["channel-threshold"].as<std::vector<int>>();
  std::vector<int> dc_offsets = sibilla::evoke ()["dc-offset"].as<std::vector<int>>();
  if (channel_thresholds.size () == 1u) for (unsigned int i = 1; i < channels.size (); i++) channel_thresholds.push_back(channel_thresholds[0]);
  if (channel_thresholds.size () != channels.size ()) ATTILA << " channels.size != channel_thresholds.size";
  if (dc_offsets.size () == 1u) for (unsigned int i = 1; i < channels.size (); i++) dc_offsets.push_back(dc_offsets[0]);
  if (dc_offsets.size () != channels.size ()) ATTILA << " channels.size != dc_offsets.size";

  board_channels_ = get<uint32_t>("/par/NumCh");
  buffer_ = new uint16_t*[board_channels_];
  for (unsigned int i = 0; i < board_channels_; i++) buffer_[i] = 0;

  set("/ch/0.." + std::to_string(board_channels_ - 1) + "/par/ChEnable", "false");
  uint32_t record_length = sibilla::evoke ()["gate-width"].as<int>();

  uint64_t channels_selftrigger_mask = 0;
  for (unsigned int i = 0; i < channels.size(); i++) {
    int ch = channels[i];
    int dc_offset = dc_offsets[i];
    int channel_threshold = channel_thresholds[i];

    if (ch > int(board_channels_) || ch < 0) ATTILA << " channel out of bound " << ch;

    buffer_[i] = new uint16_t[record_length];

    set("/ch/" + std::to_string(ch) + "/par/ChEnable", "true");
    set("/ch/" + std::to_string(ch) + "/par/DCOffset", float(dc_offset));

    if (channel_threshold < 0) continue;

    set("/ch/" + std::to_string(ch) + "/par/TriggerThrMode", "Absolute");
    set("/ch/" + std::to_string(ch) + "/par/TriggerThr", channel_threshold);
    set("/ch/" + std::to_string(ch) + "/par/SelfTriggerEdge", positive_pulse ? "RISE" : "FALL");
    channels_selftrigger_mask |= uint64_t(1) << ch;
  }

  if (channels_selftrigger_mask) {
    int majority = sibilla::evoke ()["majority"].as<int>();
    if (majority < 1 || majority > int(board_channels_)) ATTILA << " majority has to be between 1 (no majority) and # channels";
    if (majority == 1) set("/par/ITLAMainLogic", "OR");
    else {
      set("/par/ITLAMainLogic", "Majority");
      set("/par/ITLAMajorityLev", majority);
    }

    set("/par/ITLAPairLogic", "NONE");
    selftrigger_ = true;
  } else selftrigger_ = false;
  set("ITLAMask", channels_selftrigger_mask);
}

void germain::init_trigger () {
  set("/par/IOlevel", sibilla::evoke ()("nim") ? "NIM" : "TTL");
  set("/par/AcqTriggerSource", selftrigger_ ? "SwTrg|TrgIn|ITLA" : "SwTrg|TrgIn");

  uint32_t record_length = sibilla::evoke ()["gate-width"].as<int>();
  set("/par/RecordLengthS", record_length);
  set("/par/PreTriggerS", uint32_t(record_length * (1 - sibilla::evoke ()["post-trigger"].as<int>() / 100.)));
  set("/par/StartSource", "SWcmd");

  sw_trigger_ = sibilla::evoke ()("software-trigger");
}

void germain::init_buffers () {
  int ret = CAEN_FELib_GetHandle(handle_, "/endpoint/scope", &scope_);
  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_GetHandle(/endpoint/scope): " << caen_error (ret);

  //uint64_t ep_folder_handle;
  //ret = CAEN_FELib_GetParentHandle(endpoint_, NULL, &ep_folder_handle);
  //if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_GetParentHandle()" << caen_error (ret);

  ret = CAEN_FELib_SetValue(handle_, "/endpoint/par/activeendpoint", "scope");
  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_SetValue(handle_, /par/activeendpoint, scope): " << caen_error (ret);

  ret = CAEN_FELib_SetReadDataFormat(scope_, " \
        [ \
          { \"name\" : \"TIMESTAMP\", \"type\" : \"U64\" }, \
          { \"name\" : \"TRIGGER_ID\", \"type\" : \"U32\" }, \
          { \"name\" : \"WAVEFORM\", \"type\" : \"U16\", \"dim\" : 2 }, \
          { \"name\" : \"WAVEFORM_SIZE\", \"type\" : \"SIZE_T\", \"dim\" : 1 } \
        ] \
      ");
  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_SetReadDataFormat(scope_, ...): " << caen_error (ret);

  sizes_ = new size_t[board_channels_];
}

void germain::init_metadata () {
  aristotele_.board = 2740;
  aristotele_.n_bits = 16;
  aristotele_.sampling_rate = 125;
  aristotele_.gate_length = sibilla::evoke ()["gate-width"].as<int>();
  aristotele_.post_trigger = sibilla::evoke ()["post-trigger"].as<int>();
  aristotele_.threshold = sibilla::evoke ()["channel-threshold"].as<std::vector<int>>()[0];

}
std::vector<std::unique_ptr<evaristo>> germain::loop() {
  std::vector<std::unique_ptr<evaristo>> ev_v;

  for (int j = 0; j < 5; j++) {
    uint64_t timestamp;
    uint32_t trigger_id;

    int ret = CAEN_FELib_ReadData(scope_, 100, &timestamp, &trigger_id, buffer_, sizes_);
    if (ret == CAEN_FELib_Timeout) continue;
    if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_ReadData(...): " << caen_error (ret);
    
    size_t n_samples = 0;
    size_t n_channels = 0;

    int total_size = sizeof(evaristo)/sizeof(uint16_t);
    for (unsigned int k = 0; k < board_channels_; k++) { 
      total_size += sizes_[k];
      if (sizes_[k] > 0) n_samples = sizes_[k];
    }

    std::unique_ptr<evaristo> ev = std::unique_ptr<evaristo>(reinterpret_cast<evaristo*>(new uint16_t[total_size]));

    uint16_t *ptr = ev->samples;
    for (unsigned int k = 0; k < board_channels_; k++) 
      if (sizes_[k] == n_samples && buffer_[k]) {
        for (size_t z = 0; z < n_samples; z++) ptr[z] = buffer_[k][z];
        ptr += n_samples;
        n_channels ++;
      }

    ev->n_samples = n_samples;
    ev->n_channels = n_channels;
    ev->time_tag = timestamp; // use only lower 32 bits
    ev->counter = trigger_id;
    ev->unused[0] = ev->unused[1] = ev->unused[2] = ev->unused[3] = 0;
    ev->cpu_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now () - start_time_).count ();

    ev_v.push_back (std::move(ev));
  }

  if (ev_v.size () == 0 && sw_trigger_) send("/cmd/sendswtrigger"); 
  return ev_v;
}

void germain::_set (const std::string& path, const std::string& value) {
  int ret = CAEN_FELib_SetValue(handle_, path.c_str (), value.c_str ());
  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_SetValue(" << path << ", " << value << "): " << caen_error (ret);
}

void germain::send (const std::string& path) {
  int ret = CAEN_FELib_SendCommand(handle_, path.c_str ());
  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_SendCommand(" << path << "): " << caen_error (ret);
}

template<typename T> T germain::get (const std::string& path) {
  char buf[256];
  int ret = CAEN_FELib_GetValue(handle_, path.c_str (), buf);
  if (ret != CAEN_FELib_Success) ATTILA << " CAEN_FELib_GetValue(" << path << "): " << caen_error (ret);

  return boost::lexical_cast<T>(buf);
}
