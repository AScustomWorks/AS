//**************************************************************************************
//Quad QuadVCA module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code adapted from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************

#include "AS.hpp"

struct QuadVCA : Module {
	enum ParamIds {
		GAIN1_PARAM,
		GAIN2_PARAM,
		GAIN3_PARAM,
		GAIN4_PARAM,
        MODE1_PARAM,
        MODE2_PARAM,
		MODE3_PARAM,
        MODE4_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		GAIN1_CV_INPUT,
		IN1_INPUT,
		GAIN2_CV_INPUT,
		IN2_INPUT,
		GAIN3_CV_INPUT,
		IN3_INPUT,
		GAIN4_CV_INPUT,
		IN4_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		GAIN1_LIGHT,
		GAIN2_LIGHT,
		GAIN3_LIGHT,
		GAIN4_LIGHT,
		NUM_LIGHTS
	};

	float v1= 0.0;
	float v2= 0.0;
	float v3= 0.0;
	float v4= 0.0;
	const float expBase = 50.0;

	QuadVCA() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void QuadVCA::step() {
	//QuadVCA 1
	float out = 0.0;
	v1 = inputs[IN1_INPUT].value * params[GAIN1_PARAM].value;
	if(inputs[GAIN1_CV_INPUT].active){
		if(params[MODE1_PARAM].value==1){
			v1 *= clampf(inputs[GAIN1_CV_INPUT].value / 10.0, 0.0, 1.0);
		}else{
			v1 *= rescalef(powf(expBase, clampf(inputs[GAIN1_CV_INPUT].value / 10.0, 0.0, 1.0)), 1.0, expBase, 0.0, 1.0);
		}
	}
	out+=v1;
	lights[GAIN1_LIGHT].setBrightnessSmooth(fmaxf(0.0, out / 5.0));
	if (outputs[OUT1_OUTPUT].active) {
			outputs[OUT1_OUTPUT].value = out;
			out = 0.0;
	}
	//QuadVCA 2
	v2 = inputs[IN2_INPUT].value * params[GAIN2_PARAM].value;
	if(inputs[GAIN2_CV_INPUT].active){
		if(params[MODE2_PARAM].value){
			v2 *= clampf(inputs[GAIN2_CV_INPUT].value / 10.0, 0.0, 1.0);
		}else{
			v2 *= rescalef(powf(expBase, clampf(inputs[GAIN2_CV_INPUT].value / 10.0, 0.0, 1.0)), 1.0, expBase, 0.0, 1.0);
		}
	}
	out+=v2;
	lights[GAIN2_LIGHT].setBrightnessSmooth(fmaxf(0.0, out / 5.0));
	if (outputs[OUT2_OUTPUT].active) {
			outputs[OUT2_OUTPUT].value = out;
			out = 0.0;
	}
	//QuadVCA 3
	v3 = inputs[IN3_INPUT].value * params[GAIN3_PARAM].value;
	if(inputs[GAIN3_CV_INPUT].active){
		if(params[MODE3_PARAM].value){
			v3 *= clampf(inputs[GAIN3_CV_INPUT].value / 10.0, 0.0, 1.0);
		}else{
			v3 *= rescalef(powf(expBase, clampf(inputs[GAIN3_CV_INPUT].value / 10.0, 0.0, 1.0)), 1.0, expBase, 0.0, 1.0);
		}
	}
	out+=v3;
	lights[GAIN3_LIGHT].setBrightnessSmooth(fmaxf(0.0, out / 5.0));
	if (outputs[OUT3_OUTPUT].active) {
			outputs[OUT3_OUTPUT].value = out;
			out = 0.0;
	}
	//QuadVCA 4
	v4 = inputs[IN4_INPUT].value * params[GAIN4_PARAM].value;
	if(inputs[GAIN4_CV_INPUT].active){
		if(params[MODE4_PARAM].value){
			v4 *= clampf(inputs[GAIN4_CV_INPUT].value / 10.0, 0.0, 1.0);
		}else{
			v4 *= rescalef(powf(expBase, clampf(inputs[GAIN4_CV_INPUT].value / 10.0, 0.0, 1.0)), 1.0, expBase, 0.0, 1.0);
		}
	}
	out+=v4;
	lights[GAIN4_LIGHT].setBrightnessSmooth(fmaxf(0.0, out / 5.0));
	if (outputs[OUT4_OUTPUT].active) {
			outputs[OUT4_OUTPUT].value = out;
			out = 0.0;
	}
}

QuadVCAWidget::QuadVCAWidget() {
	QuadVCA *module = new QuadVCA();
	setModule(module);
    box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/QuadVCA.svg")));
		addChild(panel);
	}

	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	static const float posX[4] = {13,39,65,91};
    //SLIDERS
	addParam(createParam<as_SlidePot>(Vec(posX[0]-3, 70), module, QuadVCA::GAIN1_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<as_SlidePot>(Vec(posX[1]-3, 70), module, QuadVCA::GAIN2_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<as_SlidePot>(Vec(posX[2]-3, 70), module, QuadVCA::GAIN3_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<as_SlidePot>(Vec(posX[3]-3, 70), module, QuadVCA::GAIN4_PARAM, 0.0, 1.0, 0.0));
    //MODE SWITCHES
    addParam(createParam<as_CKSS>(Vec(posX[0], 190), module, QuadVCA::MODE1_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<as_CKSS>(Vec(posX[1], 190), module, QuadVCA::MODE2_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<as_CKSS>(Vec(posX[2], 190), module, QuadVCA::MODE3_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<as_CKSS>(Vec(posX[3], 190), module, QuadVCA::MODE4_PARAM, 0.0, 1.0, 1.0));
	//CV INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(posX[0]-4, 217), module, QuadVCA::GAIN1_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[1]-4, 217), module, QuadVCA::GAIN2_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[2]-4, 217), module, QuadVCA::GAIN3_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[3]-4, 217), module, QuadVCA::GAIN4_CV_INPUT));
	//INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(posX[0]-4, 260), module, QuadVCA::IN1_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[1]-4, 260), module, QuadVCA::IN2_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[2]-4, 260), module, QuadVCA::IN3_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(posX[3]-4, 260), module, QuadVCA::IN4_INPUT));
	//LEDS
	addChild(createLight<SmallLight<RedLight>>(Vec(posX[0]+5, 288), module, QuadVCA::GAIN1_LIGHT));//294
	addChild(createLight<SmallLight<RedLight>>(Vec(posX[1]+5, 288), module, QuadVCA::GAIN2_LIGHT));
	addChild(createLight<SmallLight<RedLight>>(Vec(posX[2]+5, 288), module, QuadVCA::GAIN3_LIGHT));
	addChild(createLight<SmallLight<RedLight>>(Vec(posX[3]+5, 288), module, QuadVCA::GAIN4_LIGHT));
	//OUTPUTS
	addOutput(createOutput<as_PJ301MPort>(Vec(posX[0]-4, 310), module, QuadVCA::OUT1_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(posX[1]-4, 310), module, QuadVCA::OUT2_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(posX[2]-4, 310), module, QuadVCA::OUT3_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(posX[3]-4, 310), module, QuadVCA::OUT4_OUTPUT));

}