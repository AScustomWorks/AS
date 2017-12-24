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

	Multiple2_5() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;
};


void Multiple2_5::step() {

	float IN1 = inputs[INPUT1].value;
	float IN2 = inputs[INPUT2].value;
	
	// Set outputs
	outputs[OUT11].value = IN1;
	outputs[OUT12].value = IN1;
	outputs[OUT13].value = IN1;
	outputs[OUT14].value = IN1;
	outputs[OUT15].value = IN1;
	outputs[OUT21].value = IN2;
	outputs[OUT22].value = IN2;
	outputs[OUT23].value = IN2;
	outputs[OUT24].value = IN2;
	outputs[OUT25].value = IN2;
	//Is it necessary to check for active outputs in this case?
	/*
	if (outputs[OUT11].active) {
		outputs[OUT11].value = IN1;
	}
	
	if (outputs[OUT12].active) {
		outputs[OUT12].value = IN1;
	}

	if (outputs[OUT13].active) {
		outputs[OUT13].value= IN1;
	}

	if (outputs[OUT14].active) {
		outputs[OUT14].value = IN1;
	}
	if (outputs[OUT15].active) {
		outputs[OUT15].value = IN1;
	}

	if (outputs[OUT21].active) {
		outputs[OUT21].value = IN2;
	}

	if (outputs[OUT22].active) {
		outputs[OUT22].value = IN2;
	}

	if (outputs[OUT23].active) {
		outputs[OUT23].value = IN2;
	}

	if (outputs[OUT24].active) {
		outputs[OUT24].value = IN2;
	}
	if (outputs[OUT25].active) {
		outputs[OUT25].value = IN2;
	}
	*/
}

Multiple2_5Widget::Multiple2_5Widget() {
	Multiple2_5 *module = new Multiple2_5();
	setModule(module);
	box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Multiple2_5.svg")));
		addChild(panel);
	}
	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(10, 65), module, Multiple2_5::INPUT1));
	addInput(createInput<as_PJ301MPort>(Vec(55, 65), module, Multiple2_5::INPUT2));
	//OUTPUTS
	addOutput(createOutput<as_PJ301MPort>(Vec(10, 130), module, Multiple2_5::OUT11));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 130), module, Multiple2_5::OUT21));
	
	addOutput(createOutput<as_PJ301MPort>(Vec(10, 175), module, Multiple2_5::OUT12));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 175), module, Multiple2_5::OUT22));

	addOutput(createOutput<as_PJ301MPort>(Vec(10, 220), module, Multiple2_5::OUT13));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 220), module, Multiple2_5::OUT23));

	addOutput(createOutput<as_PJ301MPort>(Vec(10, 265), module, Multiple2_5::OUT14));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 265), module, Multiple2_5::OUT24));

	addOutput(createOutput<as_PJ301MPort>(Vec(10, 310), module, Multiple2_5::OUT15));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 310), module, Multiple2_5::OUT25));


}
