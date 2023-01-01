#include <fstream>
#include <pstreams/pstream.h>
#include <time.h>
#include "omero.hh"
#include "attila.hh"
#include "sibilla.hh"
#include "evaristo.hh"

omero::omero (const aristotele& m) {
  o_txt_ = 0;
  o_wav_ = 0;

  if (!sibilla::evoke ()("filename")) return;

  filename_ = sibilla::evoke ()["filename"].as<std::string>();

  bool zip = sibilla::evoke ()("zip");
  bool wav = sibilla::evoke ()("wav");
  bool txt = sibilla::evoke ()("txt");
  if (filename_.size () > 3 && filename_.substr(filename_.size () - 3) == ".gz") zip = true;
  if ((filename_.size () > 4 && filename_.substr(filename_.size () - 4) == ".wav") || 
      (filename_.size () > 7 && filename_.substr(filename_.size () - 7) == ".wav.gz")) wav = true;
  if ((filename_.size () > 4 && filename_.substr(filename_.size () - 4) == ".txt") || 
      (filename_.size () > 7 && filename_.substr(filename_.size () - 7) == ".txt.gz")) txt = true;

  if (filename_.size () < 3 || filename_.substr(filename_.size () - 3) == ".gz") filename_ += ".gz";

  if (wav == txt) ATTILA << "no file format specified";


  time_t t = time (NULL);
  std::string date = ctime (&t); date.pop_back();
  if (txt) {
    if (zip) o_txt_ = std::unique_ptr<std::ostream> (new redi::opstream ("gzip - > " + filename_));
    else o_txt_ = std::unique_ptr<std::ostream> (new std::ofstream (filename_));
    *o_txt_ << "run: " << sibilla::evoke ()["run"].as<int>() << std::endl;
    *o_txt_ << "date: " << date << std::endl;;
    if (sibilla::evoke ()("comment")) *o_txt_ << "comment: " << sibilla::evoke ()["comment"].as<std::string>() << std::endl;
    *o_txt_ << m;
  } else {
    if (zip) ATTILA << "compressed wav files not supported";
    o_wav_ = std::unique_ptr<SndfileHandle> (new SndfileHandle(filename_, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, m.sampling_rate));
    if (o_wav_->error()) ATTILA << std::string("Open wav file error: ") + o_wav_->strError ();
    o_wav_->setString (SF_STR_TITLE, std::to_string(sibilla::evoke ()["run"].as<int>()).c_str ());
    o_wav_->setString (SF_STR_DATE, date.c_str ());
    if (sibilla::evoke ()("comment")) o_wav_->setString (SF_STR_COMMENT, sibilla::evoke ()["comment"].as<std::string>().c_str ());
    std::ostringstream t;
    t << m.board << " (bits = " << m.n_bits << ")";
    o_wav_->setString (SF_STR_ARTIST, t.str ().c_str ());
    o_wav_->setString (SF_STR_ALBUM, ("threshold = " + std::to_string(m.threshold)).c_str ());
    o_wav_->setString (SF_STR_TRACKNUMBER, ("gate = " + std::to_string(m.gate_length) + " post_rigger = " + std::to_string(m.post_trigger)).c_str());
  }
}

void omero::write (evaristo* ev) {
  if (!ev) return;

  if (o_txt_) *o_txt_ << *ev;
  if (o_wav_) {
    ev->marker = 0xFFFF;
    ev->header_length = sizeof(evaristo)/2; // PCM16 = 2
    o_wav_->write ((short *)ev, ev->header_length + ev->n_samples * ev->n_channels);
  }
}

