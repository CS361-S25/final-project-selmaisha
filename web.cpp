
#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

#include "emp/web/web.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/Canvas.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/init.hpp"
#include "ConfigSetup.h"
#include "World.h"

BaselineConfig config;
emp::web::Document doc("target");
// emp::web::Document settings("settings");

class BaselineAnimator : public emp::web::Animate {
  const int canvas_px = 800;

//   emp::web::Canvas canvas{canvas_px, canvas_px, "canvas"};
    emp::web::Canvas canvas{600.0, 600.0, "canvas"};

  OrgWorld world{&config};

public:
  BaselineAnimator() {
    // Setup config loading from URL or file
    auto specs = emp::ArgManager::make_builtin_specs(&config);
    emp::ArgManager am(emp::web::GetUrlParams(), specs);
    am.UseCallbacks();
    if (am.HasUnused()) std::exit(EXIT_FAILURE);

    // Show config panel
    // emp::prefab::ConfigPanel config_panel(config);
    // settings << config_panel;

    // UI
    doc << canvas;
    doc << GetToggleButton("Toggle Animation");
    doc << GetStepButton("Step");

    world.InitPopulation();
    Draw();
  }

  void DoFrame() override {
    world.Update();
    Draw();
  }

  void Draw() {
    canvas.Clear();

    const auto& pop = world.GetPopulation();
    size_t grid_side = static_cast<size_t>(std::sqrt(config.POP_SIZE()));
    double cell_w = canvas.GetWidth() / grid_side;
    double cell_h = canvas.GetHeight() / grid_side;

    for (size_t i = 0; i < pop.size(); ++i) {
      size_t x = i % grid_side;
      size_t y = i / grid_side;

      if (!pop[i]) continue;
      int genome_sum = std::accumulate(pop[i]->GetGenome().begin(), pop[i]->GetGenome().end(), 0);
      int color_val = genome_sum % 256;
      std::string color = "rgb(" + std::to_string(color_val) + ",0," + std::to_string(255 - color_val) + ")";

      canvas.Rect(x * cell_w, y * cell_h, cell_w - 1, cell_h - 1, color, "black");
    }
  }
};

BaselineAnimator animator;

int main() {
  emp::Initialize();  // Needed for Empirical web systems
  animator.Start();    // Start with one frame
}
