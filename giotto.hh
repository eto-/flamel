#include <TCanvas.h>
#include <TGraph.h>
#include <TApplication.h>
#include <memory>

class evaristo;

class giotto {
  public:
    giotto ();
    ~giotto ();

    void Draw (evaristo* ev);

  private:
    std::unique_ptr<TCanvas> window;
    std::unique_ptr<TGraph> graph;
    std::unique_ptr<TApplication> app;
};
