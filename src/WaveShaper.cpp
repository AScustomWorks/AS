//**************************************************************************************
//WaveSahper module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from HetrickCV plugins by Michael Hetrick https://github.com/mhetrick/hetrickcv
//**************************************************************************************
#include "AS.hpp"

//#include "dsp/digital.hpp"

struct WaveShaper : Module {
	enum ParamIds {
		AMOUNT_PARAM,
		SCALE_PARAM,
		RANGE_PARAM,
		BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT,
		AMOUNT_INPUT,
		BYPASS_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BYPASS_LED,
		NUM_LIGHTS
	};

	dsp::SchmittTrigger bypass_button_trig;
	dsp::SchmittTrigger bypass_cv_trig;
	bool fx_bypass = false;

	float fade_in_fx = 0.0f;
	float fade_in_dry = 0.0f;
	float fade_out_fx = 1.0f;
	float fade_out_dry = 1.0f;
    const float fade_speed = 0.001f;

	
	WaveShaper() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(WaveShaper::AMOUNT_PARAM, -5.0f, 5.0f, 0.0f, "Shape");
		configParam(WaveShaper::SCALE_PARAM, -1.0f, 1.0f, 1.0f, "Modulation");
		configParam(WaveShaper::RANGE_PARAM, 0.0f, 1.0f, 0.0f, "Range");
		configParam(WaveShaper::BYPASS_SWITCH , 0.0f, 1.0f, 0.0f, "Bypass");	
	}

	void resetFades(){
		fade_in_fx = 0.0f;
		fade_in_dry = 0.0f;
		fade_out_fx = 1.0f;
		fade_out_dry = 1.0f;
	}

	void process(const ProcessArgs &args) override {

		if (bypass_button_trig.process(params[BYPASS_SWITCH].getValue()) || bypass_cv_trig.process(inputs[BYPASS_CV_INPUT].getVoltage()) ){
			fx_bypass = !fx_bypass;
			resetFades();
		}
		lights[BYPASS_LED].value = fx_bypass ? 1.0f : 0.0f;

		float input = inputs[INPUT].getVoltage();

		bool mode5V = (params[RANGE_PARAM].getValue() == 0.0f);
		if(mode5V) input = clamp(input, -5.0f, 5.0f) * 0.2f;
		else input = clamp(input, -10.0f, 10.0f) * 0.1f;

		float shape = params[AMOUNT_PARAM].getValue() + (inputs[AMOUNT_INPUT].getVoltage() * params[SCALE_PARAM].getValue());
		shape = clamp(shape, -5.0f, 5.0f) * 0.2f;
		shape *= 0.99f;

		const float shapeB = (1.0f - shape) / (1.0f + shape);
		const float shapeA = (4.0f * shape) / ((1.0f - shape) * (1.0f + shape));

		float output = input * (shapeA + shapeB);
		output = output / ((std::abs(input) * shapeA) + shapeB);
		output *= 10.0f;
		//check for bypass switch status
		if (fx_bypass){
			fade_in_dry += fade_speed;
			if ( fade_in_dry > 1.0f ) {
				fade_in_dry = 1.0f;
			}
			fade_out_fx -= fade_speed;
			if ( fade_out_fx < 0.0f ) {
				fade_out_fx = 0.0f;
			}
			outputs[OUTPUT].setVoltage(( input * fade_in_dry ) + ( output * fade_out_fx ));
		}else{
			fade_in_fx += fade_speed;
			if ( fade_in_fx > 1.0f ) {
				fade_in_fx = 1.0f;
			}
			fade_out_dry -= fade_speed;
			if ( fade_out_dry < 0.0f ) {
				fade_out_dry = 0.0f;
			}
			outputs[OUTPUT].setVoltage(( input * fade_out_dry ) + ( output * fade_in_fx ));
		}


	}


	json_t *dataToJson()override {
		json_t *rootJm = json_object();

		json_t *statesJ = json_array();
		
			json_t *bypassJ = json_boolean(fx_bypass);
			json_array_append_new(statesJ, bypassJ);
		
		json_object_set_new(rootJm, "as_FxBypass", statesJ);

		return rootJm;
	}

	void dataFromJson(json_t *rootJm)override {
		json_t *statesJ = json_object_get(rootJm, "as_FxBypass");
		
			json_t *bypassJ = json_array_get(statesJ, 0);

			fx_bypass = !!json_boolean_value(bypassJ);
		
	}

};



struct WaveShaperWidget : ModuleWidget {

	WaveShaperWidget(WaveShaper *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WaveShaper.svg")));

		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//PARAMS
		addParam(createParam<as_KnobBlack>(Vec(26, 60), module, WaveShaper::AMOUNT_PARAM));
		addParam(createParam<as_KnobBlack>(Vec(26, 125), module, WaveShaper::SCALE_PARAM));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(33, 180), module, WaveShaper::AMOUNT_INPUT));
		//RANGE SWITCH
		addParam(createParam<as_CKSSH>(Vec(33, 220), module, WaveShaper::RANGE_PARAM));
		//BYPASS SWITCH
		addParam(createParam<LEDBezel>(Vec(55, 260), module, WaveShaper::BYPASS_SWITCH ));
		addChild(createLight<LedLight<RedLight>>(Vec(57.2, 262), module, WaveShaper::BYPASS_LED));
		//INS/OUTS
		addInput(createInput<as_PJ301MPort>(Vec(10, 310), module, WaveShaper::INPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(55, 310), module, WaveShaper::OUTPUT));

		//BYPASS CV INPUT
		addInput(createInput<as_PJ301MPort>(Vec(10, 259), module, WaveShaper::BYPASS_CV_INPUT));

	}
};



Model *modelWaveShaper = createModel<WaveShaper, WaveShaperWidget>("WaveShaper");