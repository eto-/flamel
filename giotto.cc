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

  if (graphs.size () != ev->n_channels || graphs[0]->GetN () != int(ev->n_samples)) {
    graphs.clear ();
    graphs.resize (ev->n_channels);
    for (unsigned int i = 0; i < ev->n_channels; i++) {
      graphs[i] = std::unique_ptr<TGraph>(new TGraph(ev->n_samples));
      std::iota (graphs[i]->GetX (), graphs[i]->GetX () + graphs[i]->GetN (), 0);
    }
  }

  u_int16_t up=0, down=-1;
  for (unsigned int i = 0; i < ev->n_channels; i++) {
    std::copy (ev->samples + ev->n_samples * i, ev->samples + ev->n_samples * (i + 1), graphs[i]->GetY ());
    up = std::max(*std::max_element (ev->samples + ev->n_samples * i, ev->samples + ev->n_samples * (i + 1)), up);
    down = std::min(*std::min_element (ev->samples + ev->n_samples * i, ev->samples + ev->n_samples * (i + 1)), down);
  }

  for (unsigned int i = 0; i < ev->n_channels; i++) {
    graphs[i]->GetYaxis()->SetRangeUser (down, up);
    if (i == 0) graphs[i]->Draw ("AL+");
    else graphs[i]->Draw ("L+S");
  }

  window->Update();
  gSystem->ProcessEvents();
}
