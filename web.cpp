#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

// Include necessary libraries for web interface, random number generation, and world simulation
#include "emp/math/Random.hpp"
#include "emp/prefab/Card.hpp"
#include "emp/prefab/ReadoutPanel.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/Document.hpp"
#include "emp/web/web.hpp"
#include "World.h"
#include "Org.h"
#include "emp/config/ArgManager.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/UrlParams.hpp"
#include "ConfigSetup.h"

// Declare global document objects for web interface
emp::web::Document doc("target");
emp::web::Document settings("settings");
emp::web::Document buttons("buttons");
emp::web::Document panel("panel");
emp::web::Document explanation("explanation");

// Global config for the world setup
WorldConfig config;

/**
 * input: none
 * output: none
 * purpose: Handles the animation and display of the simulation, including grid drawing, organism movement, and updates to the world.
 */
class AEAnimator : public emp::web::Animate {

    // Define arena dimensions and grid cell size
    int num_h_boxes = 30; // height of the grid (to be configurable)
    int num_w_boxes = 30; // width of the grid (to be configurable)
    const double RECT_SIDE = 15; // size of each grid cell
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};
    int update_count = 0;
    emp::Random random{config.SEED()}; // Initialize random number generator with seed
    OrgWorld world{random, &config};   // Create the world simulation

    // Initialize the canvas for drawing the world
    emp::web::Canvas canvas{width, height, "canvas"}; //might get web issue here

    public:

        /**
         * input: none
         * output: none (side-effect: initializes simulation, config panels, and web interface elements)
         * purpose: Initializes the AEAnimator, sets up world grid, injects organisms, sets up configuration panel and web interface.
         */
        AEAnimator() {
            SetupCanvasAndControls();
            ApplyConfigFromArgs();
            SetupConfigPanel();
            DisplayExplanation();
            InitializeWorld();
            SetupReadoutPanel();
        }

        /**
     * input: none
     * output: none (side-effect: updates the world state and redraws the grid)
     * purpose: Updates the world for the current frame, redrawing the grid with new state (tasks completed, etc.)
     */
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
            buttons << GetToggleButton("Toggle");
            buttons << GetStepButton("Step");
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
            config_panel.ExcludeSetting("NUM_UPDATES");
            config_panel.SetRange("NUM_BOXES", "10", "30", "1");
            config_panel.SetRange("NUM_START", "1", "100", "1");
            config_panel.SetRange("MUTATION_RATE", "0", "0.1");
            settings << config_panel;
        }

        void DisplayExplanation() {
            explanation << "This is our setup for the baseline. MORE EXPLANATIONS HERE";
        }

        void InitializeWorld() {
            random.ResetSeed(config.SEED());
            Organism* new_org;
            num_h_boxes = config.NUM_BOXES();
            num_w_boxes = config.NUM_BOXES();
            world.SetPopStruct_Grid(num_w_boxes, num_h_boxes);
            for (int i = 0; i < config.NUM_START(); i++) {
                new_org = new Organism(&world, 0);
                world.Inject(*new_org);
            }
            world.Resize(num_h_boxes, num_w_boxes);
        }

        //FIX TEXT
        void SetupReadoutPanel() {
            emp::prefab::ReadoutPanel values("Readout Values", 100);
            values.AddValues(
                "Organisms", "How many organisms are present", [this]() {
                    return emp::to_string(world.GetNumOrgs());
                },
                "NOT", "How many organisms are solving not tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().NOT_count);
                },
                "NAND", "How many organisms are solving nand tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().NAND_count);
                },
                "AND", "How many organisms are solving and tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().AND_count);
                },
                "ORN", "How many organisms are solving orn tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().ORN_count);
                },
                "OR", "How many organisms are solving or tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().OR_count);
                },
                "ANDN", "How many organisms are solving andn tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().ANDN_count);
                },
                "NOR", "How many organisms are solving nor tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().NOR_count);
                },
                "XOR", "How many organisms are solving xor tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().XOR_count);
                },
                "EQU", "How many organisms are solving equ tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().EQU_count);
                },
                "Update step", "Current update step", [this]() {
                    return emp::to_string(update_count);
                }
            );
            panel << values;
        }

};

// Instantiate the animator object to start the simulation
AEAnimator animator;

/**
 * input: none
 * output: none
 * purpose: Starts the simulation by triggering the animator step.
 */
int main() { animator.Step(); }
