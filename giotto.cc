#include <TSystem.h>
#include "giotto.hh"
#include "evaristo.hh"


giotto::giotto () {
  int arg = 0;
  app = std::unique_ptr<TApplication>(new TApplication("application",&arg,0)); 
  window = std::unique_ptr<TCanvas>(new TCanvas());
  histo = 0;
}

giotto::~giotto () {
  histo = 0;
  window = 0;
  app = 0;
}

void giotto::Draw (evaristo* ev) {
  window->cd();

  histo = std::unique_ptr<TH1D>(new TH1D("histo","Waveform",ev->n_samples,0,ev->n_samples));
  for(int i = 0; i < ev->n_samples; i++) histo->Fill(i,ev->samples[i]);
  histo->Draw();

  window->Update();
  gSystem->ProcessEvents();
}
