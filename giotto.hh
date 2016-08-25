#ifndef GIOTTO_HH
#define GIOTTO_HH

#include <TCanvas.h>
#include <TGraph.h>
#include <TApplication.h>
#include <memory>
#include <vector>

class evaristo;

class giotto {
  public:
    giotto ();
    ~giotto ();

    void draw (evaristo* ev);

  private:
    std::unique_ptr<TCanvas> window;
    std::vector<std::unique_ptr<TGraph>> graphs;
    std::unique_ptr<TApplication> app;
};

#endif
