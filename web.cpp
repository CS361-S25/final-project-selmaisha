
#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

#include "emp/web/web.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/Canvas.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/init.hpp"
#include "emp/web/UrlParams.hpp"
#include "emp/prefab/ReadoutPanel.hpp"
#include "emp/math/Random.hpp"

#include "ConfigSetup.h"
#include "World.h"
#include "Parasite.h"

WorldConfig config;

emp::web::Document doc("target");
emp::web::Document settings("settings");
emp::web::Document panel("panel");

class BaselineAnimator : public emp::web::Animate {
  const int num_w_boxes = 32;
  const int num_h_boxes = 32;
  const double RECT_SIDE = 15;
  const double width = num_w_boxes * RECT_SIDE;
  const double height = num_h_boxes * RECT_SIDE;
  int update_count = 0;

  emp::Ptr<emp::Random> random;
  emp::Ptr<OrgWorld> world;

  emp::web::Canvas canvas{width, height, "canvas"};

public:
  BaselineAnimator() {
    ApplyConfigFromArgs();  // Load from URL if available
    random.New(config.SEED());
    world.New(*random, &config);

    SetupCanvasAndControls();
    SetupConfigPanel();
    InitializeWorld();
    SetupReadoutPanel();
  }

  void DoFrame() override {
    world->Update();
    update_count++;
    Draw();
  }

private:
  void SetupCanvasAndControls() {
    doc << canvas;

    doc << GetToggleButton("Toggle");
    doc << GetStepButton("Step");


    doc << emp::web::Button([this]() {
      ResetWorldFromConfig();
    }, "Apply Settings");
  }

  void ApplyConfigFromArgs() {
    auto specs = emp::ArgManager::make_builtin_specs(&config);
    emp::ArgManager am(emp::web::GetUrlParams(), specs);
    am.UseCallbacks();  // Updates config from URL
  }

  void SetupConfigPanel() {
    emp::prefab::ConfigPanel config_panel(config);
    config_panel.ExcludeSetting("SEED");
    config_panel.ExcludeSetting("OUTPUT_DIR");
    config_panel.ExcludeSetting("RUN_TIME");
    config_panel.SetRange("NUM_START", "1", "100", "1");
    config_panel.SetRange("MUT_RATE", "0", "0.1");
    config_panel.SetRange("LIFE_SPAN", "1", "1000", "1");

    settings << config_panel;
  }

  void InitializeWorld() {
    //world->SetPopStruct_Grid(num_w_boxes, num_h_boxes);
    world->SetPopStruct_Mixed();
    world->Resize(num_h_boxes, num_w_boxes);
    // auto p = std::make_shared<Parasite>(world, 0.0);
    // world->PlaceParasite(p, 0);  // Force parasite at cell 0
    // emp::Ptr<Parasite> p = emp::NewPtr<Parasite>(world, 0.0);
    // world->InjectParasite(p);
    for (int i = 0; i < config.NUM_START(); ++i) {
      // auto* new_org = new Organism(world, 0.0);
      // world->Inject(*new_org);
      size_t pos = world->GetRandom().GetUInt(world->GetSize());  

      if (world->GetRandom().P(0.8)) {
        // auto* host = new Organism(world, 0.0);
        auto* host = new Host(world, 0.0);
        world->InjectAt(*host, pos);
      } else {
        // auto p = std::make_shared<Parasite>(world, 0.0);
        // world->PlaceParasite(p, pos);
        emp::Ptr<Parasite> p = emp::NewPtr<Parasite>(world, 0.0);
        world->InjectParasite(p);

      }
  

    }
    update_count = 0;
    Draw();
  }

  void ResetWorldFromConfig() {
    ApplyConfigFromArgs();             // Reload settings from URL
    random->ResetSeed(config.SEED());  // Reseed RNG
    world.New(*random, &config);       // Reset world
    InitializeWorld();                 // Re-inject and resize
  }

  void Draw() {
    canvas.Clear();
    const auto& pop = world->GetPopulation();
    size_t org_num = 0;

    for (int x = 0; x < num_w_boxes; ++x) {
      for (int y = 0; y < num_h_boxes; ++y) {
        if (world->IsOccupied(org_num)) {
          const auto& org_ptr = pop[org_num];
          if (org_ptr) {
            std::string color = org_ptr->GetTaskColor();  
            canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE - 1, RECT_SIDE - 1, color, "black");
          }
          if (org_ptr->HasParasite()) {
            auto parasite = org_ptr->GetParasite();
            // std::string GetTaskColor() const {
            // if (state.completed_EQU) return "purple";
            //     return "black";  // fallback color
            //   }
            std::string color = parasite->GetTaskColor();
            canvas.Circle(x * RECT_SIDE + RECT_SIDE / 2,
                          y * RECT_SIDE + RECT_SIDE / 2,
                          RECT_SIDE / 4, color, "red");
          }
          // if (world->IsParasite(org_num)) {
          //   canvas.Circle(x * RECT_SIDE + RECT_SIDE / 2,
          //                 y * RECT_SIDE + RECT_SIDE / 2,
          //                 RECT_SIDE / 4, "red");
          // }
        } else {
          canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE - 1, RECT_SIDE - 1, "white", "black");
        }
        ++org_num;
      }
    }
  }

  void SetupReadoutPanel() {
    emp::prefab::ReadoutPanel values("Readout Values", 100);
    values.AddValues(
      "Organisms", "How many organisms are present", [this]() {
        return emp::to_string(world->GetNumOrgs());
      },
      "NOT solvers", "How many organisms are solving NOT", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().NOT_count);
      },
      "NAND solvers", "How many organisms are solving NAND", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().NAND_count);
      },
      "AND solvers", "How many organisms are solving AND", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().AND_count);
      },
      "ORN solvers", "How many organisms are solving ORN", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().ORN_count);
      },
      "OR solvers", "How many organisms are solving OR", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().OR_count);
      },
      "ANDN solvers", "How many organisms are solving ANDN", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().ANDN_count);
      },
      "NOR solvers", "How many organisms are solving NOR", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().NOR_count);
      },
      "XOR solvers", "How many organisms are solving XOR", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().XOR_count);
      },
      "EQU solvers", "How many organisms are solving EQU", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().EQU_count);
      },
      "Update step", "Current update step", [this]() {
        return emp::to_string(update_count);
      },
      "Parasites", "How many parasites are present", [this]() {
        return emp::to_string(world->GetParasiteCountDataNode().GetCurrent());
      }
    );
    panel << values;
  }
};

BaselineAnimator animator;

int main() {
  emp::Initialize();
  animator.Step();  // Start paused
}