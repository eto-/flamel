#include <algorithm>
#include <numeric>
#include <TAxis.h>
#include <TSystem.h>
#include "giotto.hh"
#include "evaristo.hh"
#include "sibilla.hh"
#include <iostream>

giotto::giotto () {
  if (sibilla::evoke ()("quiet")) { window = 0; return; }

  int arg = 0;
  app = std::unique_ptr<TApplication>(new TApplication("application",&arg,0)); 
  window = std::unique_ptr<TCanvas>(new TCanvas());
}

giotto::~giotto () {
  graphs.resize(0);
  window = 0;
  app = 0;
}

void giotto::draw (evaristo* ev) {
  if (!window || !ev->n_channels) return;

  window->cd();

  int n_samples = reinterpret_cast<const evaristo::channel_data*>(ev->data)->n_samples;
  if (graphs.size () != ev->n_channels || graphs[0]->GetN () != n_samples) {
    graphs.clear ();
    graphs.resize (ev->n_channels);
    for (int i = 0; i < ev->n_channels; i++) {
      graphs[i] = std::unique_ptr<TGraph>(new TGraph(n_samples));
      std::iota (graphs[i]->GetX (), graphs[i]->GetX () + graphs[i]->GetN (), 0);
    }
  }

  u_int16_t up=0, down=-1;
  const uint16_t * ptr = reinterpret_cast<const uint16_t*>(ev->data);
  for (int i = 0; i < ev->n_channels; i++) {
    const evaristo::channel_data *d = reinterpret_cast<const evaristo::channel_data*>(ptr);
    std::copy (d->samples, d->samples + d->n_samples, graphs[i]->GetY ());
    up = std::max(*std::max_element (d->samples, d->samples + d->n_samples), up);
    down = std::min(*std::min_element (d->samples, d->samples + d->n_samples), down);
    ptr += sizeof(evaristo::channel_data)/sizeof(uint16_t) + d->n_samples;
  }

  for (int i = 0; i < ev->n_channels; i++) {
    graphs[i]->GetYaxis()->SetRangeUser (down, up);
    if (i == 0) graphs[i]->Draw ("AL+");
    else graphs[i]->Draw ("L+S");
  }

  window->Update();
  gSystem->ProcessEvents();
}
