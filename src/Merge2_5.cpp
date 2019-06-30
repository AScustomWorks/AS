//**************************************************************************************
//Multiple 2x5 module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************
#include "AS.hpp"

struct Merge2_5 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT11,
		INPUT12,
		INPUT13,
		INPUT14,
		INPUT15,
		INPUT21,
		INPUT22,
		INPUT23,
		INPUT24,
		INPUT25,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT1,
		OUTPUT2,
		NUM_OUTPUTS
	};

	Merge2_5() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	}

	void process(const ProcessArgs &args) override{

		float IN11 = inputs[INPUT11].getVoltage();
		float IN12 = inputs[INPUT12].getVoltage();
		float IN13 = inputs[INPUT13].getVoltage();
		float IN14 = inputs[INPUT14].getVoltage();
		float IN15 = inputs[INPUT15].getVoltage();
		float IN21 = inputs[INPUT21].getVoltage();
		float IN22 = inputs[INPUT22].getVoltage();
		float IN23 = inputs[INPUT23].getVoltage();
		float IN24 = inputs[INPUT24].getVoltage();
		float IN25 = inputs[INPUT25].getVoltage();

		
		// Set outputs
		outputs[OUTPUT1].setVoltage(IN11 + IN12 + IN13 + IN14 + IN15);
		outputs[OUTPUT2].setVoltage(IN21 + IN22 + IN23 + IN24 + IN25);

	}
};


struct Merge2_5Widget : ModuleWidget { 

	Merge2_5Widget(Merge2_5 *module) {
		
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Merge2_5.svg"))); 
	
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		//INPUTS 
		addInput(createInput<as_PJ301MPort>(Vec(8, 75), module, Merge2_5::INPUT11));
		addInput(createInput<as_PJ301MPort>(Vec(43, 75), module, Merge2_5::INPUT21));
		
		addInput(createInput<as_PJ301MPort>(Vec(8, 120), module, Merge2_5::INPUT12));
		addInput(createInput<as_PJ301MPort>(Vec(43, 120), module, Merge2_5::INPUT22));

		addInput(createInput<as_PJ301MPort>(Vec(8, 165), module, Merge2_5::INPUT13));
		addInput(createInput<as_PJ301MPort>(Vec(43, 165), module, Merge2_5::INPUT23));

		addInput(createInput<as_PJ301MPort>(Vec(8, 210), module, Merge2_5::INPUT14));
		addInput(createInput<as_PJ301MPort>(Vec(43, 210), module, Merge2_5::INPUT24));

		addInput(createInput<as_PJ301MPort>(Vec(8, 255), module, Merge2_5::INPUT15));
		addInput(createInput<as_PJ301MPort>(Vec(43, 255), module, Merge2_5::INPUT25));

		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(8, 310), module, Merge2_5::OUTPUT1));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(43, 310), module, Merge2_5::OUTPUT2));

	}
};


Model *modelMerge2_5 = createModel<Merge2_5, Merge2_5Widget>("Merge2_5");