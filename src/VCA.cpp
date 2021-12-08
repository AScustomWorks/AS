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

	float v1= 0.0f;
	float v2= 0.0f;
	const float expBase = 50.0f;

	bool env1_mode_cv;
	bool env2_mode_cv;

	VCA() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(VCA::LEVEL1_PARAM, 0.0f, 1.0f, 0.5f, "CH 1 Gain", "%", 0.0f, 100.0f);
		configParam(VCA::LEVEL2_PARAM, 0.0f, 1.0f, 0.5f, "CH 2 Gain", "%", 0.0f, 100.0f);

		//New in V2, config switches and ports info without displaying values
		configSwitch(MODE1_PARAM, 0.0f, 1.0f, 1.0f, "CH 1 Response", {"Exponential", "Linear"});
		configSwitch(MODE2_PARAM, 0.0f, 1.0f, 1.0f, "CH 2 Response", {"Exponential", "Linear"});
		//inputs
		configInput(ENV1_INPUT, "CH 1 Response CV");
		configInput(ENV2_INPUT, "CH 2 Response CV");
		configInput(IN1_INPUT, "CH 1");
		configInput(IN2_INPUT, "CH 2");
		//Outputs
		configOutput(OUT1_OUTPUT, "CH 1");
		configOutput(OUT2_OUTPUT, "CH 2");

	}

	void process(const ProcessArgs &args) override { 
		//VCA 1

		if(inputs[ENV1_INPUT].getVoltage()){
			env1_mode_cv =! env1_mode_cv;
			params[MODE1_PARAM].setValue(env1_mode_cv);
		}

		v1 = inputs[IN1_INPUT].getVoltage() * params[LEVEL1_PARAM].getValue();
		if(inputs[ENV1_INPUT].isConnected()){
			if(params[MODE1_PARAM].getValue()==1){
				v1 *= clamp(inputs[ENV1_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			}else{
				v1 *= rescale(powf(expBase, clamp(inputs[ENV1_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}
		outputs[OUT1_OUTPUT].setVoltage(v1);
		//VCA 2

		if(inputs[ENV2_INPUT].getVoltage()){
			env2_mode_cv =! env2_mode_cv;
			params[MODE2_PARAM].setValue(env2_mode_cv);
		}

		v2 = inputs[IN2_INPUT].getVoltage() * params[LEVEL2_PARAM].getValue();
		if(inputs[ENV2_INPUT].isConnected()){
			if(params[MODE2_PARAM].getValue()){
				v2 *= clamp(inputs[ENV2_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			}else{
				v2 *= rescale(powf(expBase, clamp(inputs[ENV2_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}
		outputs[OUT2_OUTPUT].setVoltage(v2);
	}

};

struct VCAWidget : ModuleWidget { 

	VCAWidget(VCA *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCA.svg")));
	
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//SLIDERS
		addParam(createParam<as_SlidePot>(Vec(10, 70), module, VCA::LEVEL1_PARAM));
		addParam(createParam<as_SlidePot>(Vec(55, 70), module, VCA::LEVEL2_PARAM));
		//MODE SWITCHES
		addParam(createParam<as_CKSS>(Vec(14, 190), module, VCA::MODE1_PARAM));
		addParam(createParam<as_CKSS>(Vec(59, 190), module, VCA::MODE2_PARAM));
		//PORTS
		addInput(createInput<as_PJ301MPort>(Vec(10, 217), module, VCA::ENV1_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(55, 217), module, VCA::ENV2_INPUT));

		addInput(createInput<as_PJ301MPort>(Vec(10, 260), module, VCA::IN1_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(55, 260), module, VCA::IN2_INPUT));
		
		addOutput(createOutput<as_PJ301MPortGold>(Vec(10, 310), module, VCA::OUT1_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(55, 310), module, VCA::OUT2_OUTPUT));

	}
};


Model *modelVCA = createModel<VCA, VCAWidget>("VCA");