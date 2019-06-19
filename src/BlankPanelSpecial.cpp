// Original code from CV Hetrick Blank Panel

#include "AS.hpp"

#define NUM_PANELS 5

struct BlankPanelSpecial : Module {
	enum ParamIds{
		NUM_PARAMS
	};
	enum InputIds{
		NUM_INPUTS
	};
	enum OutputIds{
		NUM_OUTPUTS
    };

    enum LightIds{
        NUM_LIGHTS
	};

    int panel = 0;

	BlankPanelSpecial() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs &args) override {}

	void onReset() override{
        panel = 0;
	}
    void onRandomize() override{
        panel = round(random::uniform() * (NUM_PANELS - 1.0f));
    }

    json_t *dataToJson() override{
		json_t *rootJ = json_object();
        json_object_set_new(rootJ, "panel", json_integer(panel));
		return rootJ;
	}
    void dataFromJson(json_t *rootJ) override{
		json_t *panelJ = json_object_get(rootJ, "panel");
		if (panelJ)
            panel = json_integer_value(panelJ);
	}
};


struct BlankPanelSpecialWidget : ModuleWidget {
    SvgPanel *panel1;
	SvgPanel *panel2;
    SvgPanel *panel3;
	SvgPanel *panel4;
    SvgPanel *panel5;

    BlankPanelSpecialWidget(BlankPanelSpecial *module) {
		setModule(module);
		box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	    panel1 = new SvgPanel();
	    panel1->box.size = box.size;
	    panel1->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blanks/as-BlankPanelSpecial0.svg")));
	    addChild(panel1);

	    panel2 = new SvgPanel();
	    panel2->box.size = box.size;
	    panel2->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blanks/as-BlankPanelSpecial1.svg")));
	    addChild(panel2);

	    panel3 = new SvgPanel();
	    panel3->box.size = box.size;
	    panel3->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blanks/as-BlankPanelSpecial2.svg")));
	    addChild(panel3);

	    panel4 = new SvgPanel();
	    panel4->box.size = box.size;
	    panel4->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blanks/as-BlankPanelSpecial3.svg")));
	    addChild(panel4);

	    panel5 = new SvgPanel();
	    panel5->box.size = box.size;
	    panel5->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blanks/as-BlankPanelSpecial4.svg")));
	    addChild(panel5);

	}
    
    void step() override {
		auto blank = dynamic_cast<BlankPanelSpecial*>(module);
		
		if(blank) {
			panel1->visible = (blank->panel == 0);
			panel2->visible = (blank->panel == 1);
		    panel3->visible = (blank->panel == 2);
			panel4->visible = (blank->panel == 3);
		    panel5->visible = (blank->panel == 4);
		}

		ModuleWidget::step();
	}

    struct Panel1Item : MenuItem{
		BlankPanelSpecial *blank;
		void onAction(const event::Action &e) override { blank->panel = 0; }
		void step() override {
			rightText = (blank->panel == 0) ? "✔" : "";
			MenuItem::step();
		}
	};

	struct Panel2Item : MenuItem{
		BlankPanelSpecial *blank;
		void onAction(const event::Action &e) override { blank->panel = 1; }
		void step() override {
			rightText = (blank->panel == 1) ? "✔" : "";
			MenuItem::step();
		}
	};

	struct Panel3Item : MenuItem{
		BlankPanelSpecial *blank;
		void onAction(const event::Action &e) override { blank->panel = 2; }
		void step() override {
			rightText = (blank->panel == 2) ? "✔" : "";
			MenuItem::step();
		}
	};

	struct Panel4Item : MenuItem{
		BlankPanelSpecial *blank;
		void onAction(const event::Action &e) override { blank->panel = 3; }
		void step() override {
			rightText = (blank->panel == 3) ? "✔" : "";
			MenuItem::step();
		}
	};

	struct Panel5Item : MenuItem{
		BlankPanelSpecial *blank;
		void onAction(const event::Action &e) override { blank->panel = 4; }
		void step() override {
			rightText = (blank->panel == 4) ? "✔" : "";
			MenuItem::step();
		}
	};

	void appendContextMenu(Menu *menu) override{
		BlankPanelSpecial *blank = dynamic_cast<BlankPanelSpecial*>(module);
		assert(blank);

		if(blank){
			menu->addChild(construct<MenuEntry>());
		    menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Not so blank panels"));
			menu->addChild(construct<Panel1Item>(&Panel1Item::text, "PANEL A", &Panel1Item::blank, blank));
		    menu->addChild(construct<Panel2Item>(&Panel2Item::text, "PANEL B", &Panel2Item::blank, blank));
		    menu->addChild(construct<Panel3Item>(&Panel3Item::text, "PANEL C", &Panel3Item::blank, blank));
		    menu->addChild(construct<Panel4Item>(&Panel4Item::text, "PANEL D", &Panel4Item::blank, blank));
		    menu->addChild(construct<Panel5Item>(&Panel5Item::text, "PANEL E", &Panel5Item::blank, blank));
		}
	}
};

Model *modelBlankPanelSpecial = createModel<BlankPanelSpecial, BlankPanelSpecialWidget>("BlankPanelSpecial");