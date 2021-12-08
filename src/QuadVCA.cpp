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

	float v1= 0.0f;
	float v2= 0.0f;
	float v3= 0.0f;
	float v4= 0.0f;
	const float expBase = 50.0f;

	bool env1_mode_cv;
	bool env2_mode_cv;
	bool env3_mode_cv;
	bool env4_mode_cv;


	QuadVCA() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(QuadVCA::GAIN1_PARAM, 0.0f, 1.0f, 0.5f, "CH 1 Gain", "%", 0.0f, 100.0f);
		configParam(QuadVCA::GAIN2_PARAM, 0.0f, 1.0f, 0.5f, "CH 2 Gain", "%", 0.0f, 100.0f);
		configParam(QuadVCA::GAIN3_PARAM, 0.0f, 1.0f, 0.5f, "CH 3 Gain", "%", 0.0f, 100.0f);
		configParam(QuadVCA::GAIN4_PARAM, 0.0f, 1.0f, 0.5f, "CH 4 Gain", "%", 0.0f, 100.0f);

		//New in V2, config switches and ports info without displaying values
		configSwitch(MODE1_PARAM, 0.0f, 1.0f, 1.0f, "CH 1 Response", {"Exponential", "Linear"});
		configSwitch(MODE2_PARAM, 0.0f, 1.0f, 1.0f, "CH 2 Response", {"Exponential", "Linear"});
		configSwitch(MODE3_PARAM, 0.0f, 1.0f, 1.0f, "CH 3 Response", {"Exponential", "Linear"});
		configSwitch(MODE4_PARAM, 0.0f, 1.0f, 1.0f, "CH 4 Response", {"Exponential", "Linear"});
		//inputs
		configInput(GAIN1_CV_INPUT, "CH 1 Response CV");
		configInput(GAIN2_CV_INPUT, "CH 2 Response CV");
		configInput(GAIN3_CV_INPUT, "CH 3 Response CV");
		configInput(GAIN4_CV_INPUT, "CH 4 Response CV");
		configInput(IN1_INPUT, "CH 1");
		configInput(IN2_INPUT, "CH 2");
		configInput(IN3_INPUT, "CH 3");
		configInput(IN4_INPUT, "CH 4");
		//Outputs
		configOutput(OUT1_OUTPUT, "CH 1");
		configOutput(OUT2_OUTPUT, "CH 2");
		configOutput(OUT3_OUTPUT, "CH 3");
		configOutput(OUT4_OUTPUT, "CH 4");





	}

	void process(const ProcessArgs &args) override {
		//QuadVCA 1
		float out = 0.0;
		v1 = inputs[IN1_INPUT].getVoltage() * params[GAIN1_PARAM].getValue();

		if(inputs[GAIN1_CV_INPUT].getVoltage()){
			env1_mode_cv =! env1_mode_cv;
			params[MODE1_PARAM].setValue(env1_mode_cv);
		}
		if(inputs[GAIN1_CV_INPUT].isConnected()){
			if(params[MODE1_PARAM].getValue()==1){
				v1 *= clamp(inputs[GAIN1_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			}else{
				v1 *= rescale(powf(expBase, clamp(inputs[GAIN1_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}
		out+=v1;
		lights[GAIN1_LIGHT].setSmoothBrightness(fmaxf(0.0f, out / 5.0f), args.sampleTime);
		if (outputs[OUT1_OUTPUT].isConnected()) {
				outputs[OUT1_OUTPUT].setVoltage(out);
				out = 0.0f;
		}
		//QuadVCA 2
		v2 = inputs[IN2_INPUT].getVoltage() * params[GAIN2_PARAM].getValue();

		if(inputs[GAIN2_CV_INPUT].getVoltage()){
			env2_mode_cv =! env2_mode_cv;
			params[MODE2_PARAM].setValue(env2_mode_cv);
		}
		if(inputs[GAIN2_CV_INPUT].isConnected()){
			if(params[MODE2_PARAM].getValue()){
				v2 *= clamp(inputs[GAIN2_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			}else{
				v2 *= rescale(powf(expBase, clamp(inputs[GAIN2_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}
		out+=v2;
		lights[GAIN2_LIGHT].setSmoothBrightness(fmaxf(0.0f, out / 5.0f), args.sampleTime);
		if (outputs[OUT2_OUTPUT].isConnected()) {
				outputs[OUT2_OUTPUT].setVoltage(out);
				out = 0.0f;
		}
		//QuadVCA 3
		v3 = inputs[IN3_INPUT].getVoltage() * params[GAIN3_PARAM].getValue();
		if(inputs[GAIN3_CV_INPUT].getVoltage()){
			env3_mode_cv =! env3_mode_cv;
			params[MODE3_PARAM].setValue(env3_mode_cv);
		}		
		if(inputs[GAIN3_CV_INPUT].isConnected()){
			if(params[MODE3_PARAM].getValue()){
				v3 *= clamp(inputs[GAIN3_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			}else{
				v3 *= rescale(powf(expBase, clamp(inputs[GAIN3_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}
		out+=v3;
		lights[GAIN3_LIGHT].setSmoothBrightness(fmaxf(0.0f, out / 5.0f), args.sampleTime);
		if (outputs[OUT3_OUTPUT].isConnected()) {
				outputs[OUT3_OUTPUT].setVoltage(out);
				out = 0.0f;
		}
		//QuadVCA 4
		v4 = inputs[IN4_INPUT].getVoltage() * params[GAIN4_PARAM].getValue();
		if(inputs[GAIN4_CV_INPUT].getVoltage()){
			env4_mode_cv =! env4_mode_cv;
			params[MODE4_PARAM].setValue(env4_mode_cv);
		}
		if(inputs[GAIN4_CV_INPUT].isConnected()){
			if(params[MODE4_PARAM].getValue()){
				v4 *= clamp(inputs[GAIN4_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			}else{
				v4 *= rescale(powf(expBase, clamp(inputs[GAIN4_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f)), 1.0f, expBase, 0.0f, 1.0f);
			}
		}
		out+=v4;
		lights[GAIN4_LIGHT].setSmoothBrightness(fmaxf(0.0f, out / 5.0f), args.sampleTime);
		if (outputs[OUT4_OUTPUT].isConnected()) {
				outputs[OUT4_OUTPUT].setVoltage(out);
				out = 0.0f;
		}
	}
};


struct QuadVCAWidget : ModuleWidget { 

	QuadVCAWidget(QuadVCA *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/QuadVCA.svg")));
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		static const float posX[4] = {13,39,65,91};
		//SLIDERS
		addParam(createParam<as_SlidePot>(Vec(posX[0]-3, 70), module, QuadVCA::GAIN1_PARAM));
		addParam(createParam<as_SlidePot>(Vec(posX[1]-3, 70), module, QuadVCA::GAIN2_PARAM));
		addParam(createParam<as_SlidePot>(Vec(posX[2]-3, 70), module, QuadVCA::GAIN3_PARAM));
		addParam(createParam<as_SlidePot>(Vec(posX[3]-3, 70), module, QuadVCA::GAIN4_PARAM));
		//MODE SWITCHES
		addParam(createParam<as_CKSS>(Vec(posX[0], 190), module, QuadVCA::MODE1_PARAM));
		addParam(createParam<as_CKSS>(Vec(posX[1], 190), module, QuadVCA::MODE2_PARAM));
		addParam(createParam<as_CKSS>(Vec(posX[2], 190), module, QuadVCA::MODE3_PARAM));
		addParam(createParam<as_CKSS>(Vec(posX[3], 190), module, QuadVCA::MODE4_PARAM));
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
		addChild(createLight<SmallLight<RedLight>>(Vec(posX[0]+5, 288), module, QuadVCA::GAIN1_LIGHT));
		addChild(createLight<SmallLight<RedLight>>(Vec(posX[1]+5, 288), module, QuadVCA::GAIN2_LIGHT));
		addChild(createLight<SmallLight<RedLight>>(Vec(posX[2]+5, 288), module, QuadVCA::GAIN3_LIGHT));
		addChild(createLight<SmallLight<RedLight>>(Vec(posX[3]+5, 288), module, QuadVCA::GAIN4_LIGHT));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[0]-4, 310), module, QuadVCA::OUT1_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[1]-4, 310), module, QuadVCA::OUT2_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[2]-4, 310), module, QuadVCA::OUT3_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(posX[3]-4, 310), module, QuadVCA::OUT4_OUTPUT));

	}
};


Model *modelQuadVCA = createModel<QuadVCA, QuadVCAWidget>("QuadVCA");