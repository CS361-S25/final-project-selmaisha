
#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

#include "emp/web/web.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/Canvas.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/init.hpp"
#include "emp/web/UrlParams.hpp"
#include "emp/prefab/ReadoutPanel.hpp"

// #include "emp/prefab/ReadoutPanel.hpp"

#include "emp/math/Random.hpp"
#include "ConfigSetup.h"
#include "World.h"

BaselineConfig config;

emp::web::Document doc("target");
emp::web::Document settings("settings");
// emp::web::Document buttons("buttons");
emp::web::Document panel("panel");
// emp::web::Document explanation("explanation");


class BaselineAnimator : public emp::web::Animate {
  const int num_w_boxes = 32;
  const int num_h_boxes = 32;
  const double RECT_SIDE = 15;
  const double width = num_w_boxes * RECT_SIDE;
  const double height = num_h_boxes * RECT_SIDE;
  int update_count = 0; // Number of updates

  emp::Random random{config.SEED()}; // Initialize random number generator with seed
  OrgWorld world{random, &config};   // Create the world simulation

    // Initialize the canvas for drawing the world
    emp::web::Canvas canvas{width, height, "canvas"};
    // emp::Ptr<emp::Random> random;  // Make it a pointer
    // emp::Ptr<OrgWorld> world;
    

public:
  BaselineAnimator() {

    ApplyConfigFromArgs();
    // random.New(config.SEED());                 // Initialize after config is loaded
    // world.New(*random, &config); 
    SetupCanvasAndControls();
    
    SetupConfigPanel();
    InitializeWorld();
    SetupReadoutPanel();
  }

  void DoFrame() override {
    world.Update();
    update_count++;
    Draw();
  }

  void Draw() {
    canvas.Clear();
    const auto& pop = world.GetPopulation();
    size_t org_num = 0;

    for (int x = 0; x < num_w_boxes; ++x) {
      for (int y = 0; y < num_h_boxes; ++y) {
        if (world.IsOccupied(org_num)) {
          const auto& org_ptr = pop[org_num];
          if (org_ptr) {
            std::string color = org_ptr->GetTaskColor();  
            canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE - 1, RECT_SIDE - 1, color, "black");
          }
        } else {
          canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE - 1, RECT_SIDE - 1, "white", "black");
        }
        ++org_num;
      }
    }
  }

  private:
        void SetupCanvasAndControls() {
            doc << canvas;
            doc << GetToggleButton("Toggle");
            doc << GetStepButton("Step");
        }

        void ApplyConfigFromArgs() {
            auto specs = emp::ArgManager::make_builtin_specs(&config);
            emp::ArgManager am(emp::web::GetUrlParams(), specs);
            am.UseCallbacks();
            if (am.HasUnused()) std::exit(EXIT_FAILURE);
        }

        void SetupConfigPanel() {
            emp::prefab::ConfigPanel config_panel(config);
            config_panel.ExcludeSetting("SEED");
            config_panel.ExcludeSetting("RUN_TIME");
            config_panel.ExcludeSetting("OUTPUT_DIR");
            config_panel.SetRange("NUM_START", "1", "100", "1");
            config_panel.SetRange("MUTATION_RATE", "0", "0.1");
            settings << config_panel;
        }


        void InitializeWorld() {
            random.ResetSeed(config.SEED());
            Organism* new_org;
            world.SetPopStruct_Grid(num_w_boxes, num_h_boxes);
            for (int i = 0; i < config.NUM_START(); i++) {
                new_org = new Organism(&world, 0);
                world.Inject(*new_org);
            }
            world.Resize(num_h_boxes, num_w_boxes);
        }
        // void InitializeWorld() {
        //   world->SetPopStruct_Grid(num_w_boxes, num_h_boxes);
        //   world->Resize(num_h_boxes, num_w_boxes);
        //   for (int i = 0; i < config.NUM_START(); ++i) {
        //     auto* new_org = new Organism(world, 0.0);
        //     world->Inject(*new_org);
        //   }
        // }

        void SetupReadoutPanel() {
          emp::prefab::ReadoutPanel values("Readout Values", 100);
          values.AddValues(
              "Organisms", "How many organisms are present", [this]() {
                  return emp::to_string(world.GetNumOrgs());
              },
              "NOT solvers", "How many organisms are solving NOT", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().NOT_count);
              },
              "NAND solvers", "How many organisms are solving NAND", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().NAND_count);
              },
              "AND solvers", "How many organisms are solving AND", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().AND_count);
              },
              "ORN solvers", "How many organisms are solving ORN", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().ORN_count);
              },
              "OR solvers", "How many organisms are solving OR", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().OR_count);
              },
              "ANDN solvers", "How many organisms are solving ANDN", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().ANDN_count);
              },
              "NOR solvers", "How many organisms are solving NOR", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().NOR_count);
              },
              "XOR solvers", "How many organisms are solving XOR", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().XOR_count);
              },
              "EQU solvers", "How many organisms are solving EQU", [this]() {
                  return emp::to_string(world.GetAllTaskSolvers().EQU_count);
              },
              "Update step", "Current update step", [this]() {
                  return emp::to_string(update_count);
              }
          );
          panel << values;
      }


};

BaselineAnimator animator;

int main() {
  // emp::Initialize();
  animator.Step();
}
