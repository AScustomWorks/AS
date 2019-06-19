//**************************************************************************************
//Multiple 2x5 module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************
#include "AS.hpp"

struct Multiple2_5 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT1,
		INPUT2,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT11,
		OUT12,
		OUT13,
		OUT14,
		OUT15,
		OUT21,
		OUT22,
		OUT23,
		OUT24,
		OUT25,
		NUM_OUTPUTS
	};

	Multiple2_5() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	}

	void process(const ProcessArgs &args) override {

		float IN1 = inputs[INPUT1].getVoltage();
		float IN2 = inputs[INPUT2].getVoltage();
		
		// Set outputs
		outputs[OUT11].setVoltage(IN1);
		outputs[OUT12].setVoltage(IN1);
		outputs[OUT13].setVoltage(IN1);
		outputs[OUT14].setVoltage(IN1);
		outputs[OUT15].setVoltage(IN1);
		/* Update suggested by MarcBrule
		If the input 2 is not active the second row of outputs takes the value of input 1
		so the module becomes a multi 1 x 10, nice and simple idea!
		*/
		if(!inputs[INPUT2].isConnected()){
			outputs[OUT21].setVoltage(IN1);
			outputs[OUT22].setVoltage(IN1);
			outputs[OUT23].setVoltage(IN1);
			outputs[OUT24].setVoltage(IN1);
			outputs[OUT25].setVoltage(IN1);

		}else{
			outputs[OUT21].setVoltage(IN2);
			outputs[OUT22].setVoltage(IN2);
			outputs[OUT23].setVoltage(IN2);
			outputs[OUT24].setVoltage(IN2);
			outputs[OUT25].setVoltage(IN2);
		}
		/*
		outputs[OUT21].setVoltage(IN2);
		outputs[OUT22].setVoltage(IN2);
		outputs[OUT23].setVoltage(IN2);
		outputs[OUT24].setVoltage(IN2);
		outputs[OUT25].setVoltage(IN2);
		*/
		//Is it necessary to check for active outputs in this case?
		/*
		if (outputs[OUT11].isConnected()) {
			outputs[OUT11].setVoltage(IN1);
		}
		
		if (outputs[OUT12].isConnected()) {
			outputs[OUT12].setVoltage(IN1);
		}

		if (outputs[OUT13].isConnected()) {
			outputs[OUT13].value= IN1;
		}

		if (outputs[OUT14].isConnected()) {
			outputs[OUT14].setVoltage(IN1);
		}
		if (outputs[OUT15].isConnected()) {
			outputs[OUT15].setVoltage(IN1);
		}

		if (outputs[OUT21].isConnected()) {
			outputs[OUT21].setVoltage(IN2);
		}

		if (outputs[OUT22].isConnected()) {
			outputs[OUT22].setVoltage(IN2);
		}

		if (outputs[OUT23].isConnected()) {
			outputs[OUT23].setVoltage(IN2);
		}

		if (outputs[OUT24].isConnected()) {
			outputs[OUT24].setVoltage(IN2);
		}
		if (outputs[OUT25].isConnected()) {
			outputs[OUT25].setVoltage(IN2);
		}
		*/
	}
};



struct Multiple2_5Widget : ModuleWidget { 

	Multiple2_5Widget(Multiple2_5 *module) {
		
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Multiple2_5.svg")));
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(8, 65), module, Multiple2_5::INPUT1));
		addInput(createInput<as_PJ301MPort>(Vec(43, 65), module, Multiple2_5::INPUT2));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPort>(Vec(8, 130), module, Multiple2_5::OUT11));
		addOutput(createOutput<as_PJ301MPort>(Vec(43, 130), module, Multiple2_5::OUT21));
		
		addOutput(createOutput<as_PJ301MPort>(Vec(8, 175), module, Multiple2_5::OUT12));
		addOutput(createOutput<as_PJ301MPort>(Vec(43, 175), module, Multiple2_5::OUT22));

		addOutput(createOutput<as_PJ301MPort>(Vec(8, 220), module, Multiple2_5::OUT13));
		addOutput(createOutput<as_PJ301MPort>(Vec(43, 220), module, Multiple2_5::OUT23));

		addOutput(createOutput<as_PJ301MPort>(Vec(8, 265), module, Multiple2_5::OUT14));
		addOutput(createOutput<as_PJ301MPort>(Vec(43, 265), module, Multiple2_5::OUT24));

		addOutput(createOutput<as_PJ301MPort>(Vec(8, 310), module, Multiple2_5::OUT15));
		addOutput(createOutput<as_PJ301MPort>(Vec(43, 310), module, Multiple2_5::OUT25));

	}
};


Model *modelMultiple2_5 = createModel<Multiple2_5, Multiple2_5Widget>("Multiple2_5");