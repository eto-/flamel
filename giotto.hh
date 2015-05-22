#ifndef GIOTTO_HH
#define GIOTTO_HH

#include <TCanvas.h>
#include <TGraph.h>
#include <TApplication.h>
#include <memory>

class evaristo;

class giotto {
  public:
    giotto ();
    ~giotto ();

    void draw (evaristo* ev);

  private:
    std::unique_ptr<TCanvas> window;
    std::unique_ptr<TGraph> graph;
    std::unique_ptr<TApplication> app;
};

#endif
