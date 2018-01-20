#include "AS.hpp"

struct BlankPanel8 : Module {
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

	BlankPanel8() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void BlankPanel8::step() 
{
}

BlankPanel8Widget::BlankPanel8Widget() 
{
	BlankPanel8 *module = new BlankPanel8();
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/BlankPanel8.svg")));
		panel->box.size = box.size;
		addChild(panel);
	}

	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}