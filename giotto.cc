#include <algorithm>
#include <TSystem.h>
#include "giotto.hh"
#include "evaristo.hh"

giotto::giotto () {
  int arg = 0;
  app = std::unique_ptr<TApplication>(new TApplication("application",&arg,0)); 
  window = std::unique_ptr<TCanvas>(new TCanvas());
  graph = 0;
}

giotto::~giotto () {
  graph = 0;
  window = 0;
  app = 0;
}

void giotto::Draw (evaristo* ev) {
  window->cd();

  if (!graph || graph->GetN () != ev->n_samples) {
    graph = std::unique_ptr<TGraph>(new TGraph(ev->n_samples));
    std::iota (graph->GetX (), graph->GetX () + graph->GetN (), 0);
  }
  std::copy (ev->samples, ev->samples + ev->n_samples, graph->GetY ());
  graph->Draw ("AL+");

  window->Update();
  gSystem->ProcessEvents();
}
