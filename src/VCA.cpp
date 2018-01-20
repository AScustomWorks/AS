//**************************************************************************************
//VCA module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************

#include "AS.hpp"

struct VCA : Module {
	enum ParamIds {
		LEVEL1_PARAM,
		LEVEL2_PARAM,
        MODE1_PARAM,
        MODE2_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ENV1_INPUT,
		IN1_INPUT,
		ENV2_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	float v1= 0.0;
	float v2= 0.0;
	const float expBase = 50.0;

	VCA() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;
};

void VCA::step() {
	//VCA 1
	v1 = inputs[IN1_INPUT].value * params[LEVEL1_PARAM].value;
	if(inputs[ENV1_INPUT].active){
		if(params[MODE1_PARAM].value==1){
			v1 *= clampf(inputs[ENV1_INPUT].value / 10.0, 0.0, 1.0);
		}else{
			v1 *= rescalef(powf(expBase, clampf(inputs[ENV1_INPUT].value / 10.0, 0.0, 1.0)), 1.0, expBase, 0.0, 1.0);
		}
	}
	outputs[OUT1_OUTPUT].value = v1;
	//VCA 2
	v2 = inputs[IN2_INPUT].value * params[LEVEL2_PARAM].value;
	if(inputs[ENV2_INPUT].active){
		if(params[MODE2_PARAM].value){
			v2 *= clampf(inputs[ENV2_INPUT].value / 10.0, 0.0, 1.0);
		}else{
			v2 *= rescalef(powf(expBase, clampf(inputs[ENV2_INPUT].value / 10.0, 0.0, 1.0)), 1.0, expBase, 0.0, 1.0);
		}
	}
	outputs[OUT2_OUTPUT].value = v2;
}

VCAWidget::VCAWidget() {
	VCA *module = new VCA();
	setModule(module);
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/VCA.svg")));
		addChild(panel);
	}

	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    //SLIDERS
	addParam(createParam<as_SlidePot>(Vec(10, 70), module, VCA::LEVEL1_PARAM, 0.0, 1.0, 0.5));
	addParam(createParam<as_SlidePot>(Vec(55, 70), module, VCA::LEVEL2_PARAM, 0.0, 1.0, 0.5));
    //MODE SWITCHES
    addParam(createParam<as_CKSS>(Vec(14, 190), module, VCA::MODE1_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<as_CKSS>(Vec(59, 190), module, VCA::MODE2_PARAM, 0.0, 1.0, 1.0));
	//PORTS
	addInput(createInput<as_PJ301MPort>(Vec(10, 217), module, VCA::ENV1_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(55, 217), module, VCA::ENV2_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(10, 260), module, VCA::IN1_INPUT));
    addInput(createInput<as_PJ301MPort>(Vec(55, 260), module, VCA::IN2_INPUT));
	
	addOutput(createOutput<as_PJ301MPort>(Vec(10, 310), module, VCA::OUT1_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 310), module, VCA::OUT2_OUTPUT));

}