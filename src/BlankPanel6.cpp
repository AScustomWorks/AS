#include "AS.hpp"

struct BlankPanel6 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	BlankPanel6() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override {
	}
};

struct BlankPanel6Widget : ModuleWidget { 

	BlankPanel6Widget(BlankPanel6 *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blanks/BlankPanel6.svg")));
		
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}
};



Model *modelBlankPanel6 = createModel<BlankPanel6, BlankPanel6Widget>("BlankPanel6");