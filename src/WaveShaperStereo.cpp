//**************************************************************************************
//WaveShaperStereo module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Code taken from HetrickCV plugins by Michael Hetrick https://github.com/mhetrick/hetrickcv
//**************************************************************************************
#include "AS.hpp"

//#include "dsp/digital.hpp"

struct WaveShaperStereo : Module {
	enum ParamIds {
		SHAPE_PARAM,
		SCALE_PARAM,
		RANGE_PARAM,
		BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		SIGNAL_INPUT_L,
		SIGNAL_INPUT_R,
		SCALE_CV_INPUT,
		SHAPE_CV_INPUT,
		WAVE_MOD_INPUT,
		RANGE_CV_INPUT,
		BYPASS_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SIGNAL_OUTPUT_L,
		SIGNAL_OUTPUT_R,
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

	float input_signal_L = 0.0f;
	float output_signal_L = 0.0f;
	float input_signal_R = 0.0f;
	float output_signal_R = 0.0f;

	bool mono_signal = false;
	bool voltage_mode_cv = false;
	float shape_value = 0.0f;
	float scale_cv_value = 0.0f;
	float bypass_signal_L = 0.0f;
	float bypass_signal_R = 0.0f;
	
	WaveShaperStereo() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(WaveShaperStereo::SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "Shape", "%", 0.0f, 100.0f);
		configParam(WaveShaperStereo::SCALE_PARAM, -1.0f, 1.0f, 0.0f, "Scale", "%", 0.0f, 100.0f);

		//New in V2, config switches info without displaying values
		configSwitch(RANGE_PARAM, 0.0f, 1.0f, 0.0f, "Range", {"-/+ 10V", "-/+ 5V"});

		//New in V2, config temporary buttons info without displaying values
		configButton(BYPASS_SWITCH, "Bypass");
		//new V2, port labels
		//Inputs
		configInput(SCALE_CV_INPUT, "Scale CV");
		configInput(SHAPE_CV_INPUT, "Shape CV");
		configInput(WAVE_MOD_INPUT, "Wave Mod CV");
		configInput(RANGE_CV_INPUT, "Range CV");
		
		configInput(SIGNAL_INPUT_L, "Left audio");
		configInput(SIGNAL_INPUT_R, "Right audio");

		configInput(BYPASS_CV_INPUT, "Bypass CV");
		//Outputs
		configOutput(SIGNAL_OUTPUT_L, "Left audio");
		configOutput(SIGNAL_OUTPUT_R, "Right audio");

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

		input_signal_L = inputs[SIGNAL_INPUT_L].getVoltage();
		bypass_signal_L = input_signal_L;

		if(!inputs[SIGNAL_INPUT_R].isConnected()){
			mono_signal = true;
			input_signal_R = input_signal_L;
			bypass_signal_R = input_signal_R;
		}else{
			mono_signal = false;
			input_signal_R = inputs[SIGNAL_INPUT_R].getVoltage();
			bypass_signal_R = input_signal_R;
		}

		if(inputs[RANGE_CV_INPUT].getVoltage()){
			voltage_mode_cv =! voltage_mode_cv;
			params[RANGE_PARAM].setValue(voltage_mode_cv);
		}
		

		bool mode5V = params[RANGE_PARAM].getValue();

		if(mode5V){
			input_signal_L = clamp(input_signal_L, -5.0f, 5.0f) * 0.2f;
			if(mono_signal){
				input_signal_R = input_signal_L;
			}else{
				input_signal_R = clamp(input_signal_R, -5.0f, 5.0f) * 0.2f;
			}
		}else{
			input_signal_L = clamp(input_signal_L, -10.0f, 10.0f) * 0.1f;
			if(mono_signal){
				input_signal_R = input_signal_L;
			}else{
				input_signal_R = clamp(input_signal_R, -10.0f, 10.0f) * 0.1f;
			}
		}

		//shape_value = rescale(params[SHAPE_PARAM].getValue(),-1.0f, 1.0f, -5.0f, 5.0f);
		shape_value = clamp( rescale(inputs[SHAPE_CV_INPUT].getVoltage(),-10.0f, 10.0f, -5.0f, 5.0f) + rescale(params[SHAPE_PARAM].getValue(),-1.0f, 1.0f, -5.0f, 5.0f), -10.0f,10.0f);

		scale_cv_value = clamp(params[SCALE_PARAM].getValue() + inputs[SCALE_CV_INPUT].getVoltage() / 10.0f, -1.0f, 1.0f);

		float shape = shape_value + (inputs[WAVE_MOD_INPUT].getVoltage() * scale_cv_value);
		//float shape = shape_value + (inputs[SHAPE_CV_INPUT].getVoltage() * params[SCALE_PARAM].getValue());
		shape = clamp(shape, -5.0f, 5.0f) * 0.2f;
		shape *= 0.99f;

		const float shapeB = (1.0f - shape) / (1.0f + shape);
		const float shapeA = (4.0f * shape) / ((1.0f - shape) * (1.0f + shape));

		output_signal_L = input_signal_L * (shapeA + shapeB);
		output_signal_L = output_signal_L / ((std::abs(input_signal_L) * shapeA) + shapeB);

		if(mono_signal){
			output_signal_R = output_signal_L;
		}else{
			output_signal_R = input_signal_R * (shapeA + shapeB);
			output_signal_R = output_signal_R / ((std::abs(input_signal_R) * shapeA) + shapeB);
		}
		//output *= 10.0f;

		if(mode5V){
			output_signal_L *= 5.0f;
			output_signal_R *= 5.0f;
		} else {
			output_signal_L *= 10.0f;
			output_signal_R *= 10.0f;
		}
		

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
			outputs[SIGNAL_OUTPUT_L].setVoltage(  ( bypass_signal_L * fade_in_dry ) + ( (output_signal_L) * fade_out_fx ) );
			outputs[SIGNAL_OUTPUT_R].setVoltage(  ( bypass_signal_R * fade_in_dry ) + ( (output_signal_R) * fade_out_fx ) );
		}else{
			fade_in_fx += fade_speed;
			if ( fade_in_fx > 1.0f ) {
				fade_in_fx = 1.0f;
			}
			fade_out_dry -= fade_speed;
			if ( fade_out_dry < 0.0f ) {
				fade_out_dry = 0.0f;
			}
			outputs[SIGNAL_OUTPUT_L].setVoltage(( bypass_signal_L * fade_out_dry ) + ( output_signal_L * fade_in_fx ));
			outputs[SIGNAL_OUTPUT_R].setVoltage(( bypass_signal_R * fade_out_dry ) + ( output_signal_R * fade_in_fx ));
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



struct WaveShaperStereoWidget : ModuleWidget {

	WaveShaperStereoWidget(WaveShaperStereo *module) {

		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/WaveShaperStereo.svg")));

		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//PARAMS
		addParam(createParam<as_FxKnobBlack>(Vec(43, 60), module, WaveShaperStereo::SHAPE_PARAM));
		addParam(createParam<as_FxKnobBlack>(Vec(43, 125), module, WaveShaperStereo::SCALE_PARAM));
		//RANGE SWITCH
		addParam(createParam<as_CKSSH>(Vec(33, 220), module, WaveShaperStereo::RANGE_PARAM));
		//CV INPUTS		
		addInput(createInput<as_PJ301MPort>(Vec(10, 67), module, WaveShaperStereo::SHAPE_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(10, 110), module, WaveShaperStereo::SCALE_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(33, 182), module, WaveShaperStereo::RANGE_CV_INPUT));
		//BYPASS SWITCH
		addParam(createParam<LEDBezel>(Vec(55, 260), module, WaveShaperStereo::BYPASS_SWITCH));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(57.2, 262), module, WaveShaperStereo::BYPASS_LED));
		//BYPASS CV INPUT
		addInput(createInput<as_PJ301MPort>(Vec(10, 259), module, WaveShaperStereo::BYPASS_CV_INPUT));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(10, 152), module, WaveShaperStereo::WAVE_MOD_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(15, 300), module, WaveShaperStereo::SIGNAL_INPUT_L));
		addInput(createInput<as_PJ301MPort>(Vec(15, 330), module, WaveShaperStereo::SIGNAL_INPUT_R));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(50, 300), module, WaveShaperStereo::SIGNAL_OUTPUT_L));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(50, 330), module, WaveShaperStereo::SIGNAL_OUTPUT_R));

	}
};



Model *modelWaveShaperStereo = createModel<WaveShaperStereo, WaveShaperStereoWidget>("WaveShaperStereo");