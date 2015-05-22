#include <TCanvas.h>
#include <TH1D.h>
#include <TApplication.h>
#include <memory>

class evaristo;

class giotto {
  public:
    giotto();

    void Draw (evaristo* ev);

  private:
    std::unique_ptr<TCanvas> window;
    std::unique_ptr<TH1D> histo;
    std::unique_ptr<TApplication> app;
};
