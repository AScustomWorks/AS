//***********************************************************************************************
//
//TremoloStereoFx module for VCV Rack by Alfredo Santamaria  - AS - https://github.com/AScustomWorks/AS
//
//LFO code adapted from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//***********************************************************************************************

#include "AS.hpp"
//#include "dsp/digital.hpp"

//LFO CODE *****************************
struct LowFrequencyoscillator {
	float phase = 0.0f;
	float pw = 0.5f;
	float freq = 1.0f;
	bool offset = false;
	bool invert = false;
	dsp::SchmittTrigger resetTrigger;

	LowFrequencyoscillator() {

	}
	void setPitch(float pitch) {
		pitch = fminf(pitch, 8.0f);
		freq = powf(2.0f, pitch);
	}
	void setPulseWidth(float pw_) {
		const float pwMin = 0.01f;
		pw = clamp(pw_, pwMin, 1.0f - pwMin);
	}
	void setReset(float reset) {
		if (resetTrigger.process(reset)) {
			phase = 0.0f;
		}
	}
	void step(float dt) {
		float deltaPhase = fminf(freq * dt, 0.5f);
		phase += deltaPhase;
		if (phase >= 1.0f)
			phase -= 1.0f;
	}
	float sin() {
		if (offset)
			return 1.0f - cosf(2*M_PI * phase) * (invert ? -1.0f : 1.0f);
		else
			return sinf(2.0f*M_PI * phase) * (invert ? -1.0f : 1.0f);
	}
	float tri(float x) {
		return 4.0f * fabsf(x - roundf(x));
	}
	float tri() {
		if (offset)
			return tri(invert ? phase - 0.5 : phase);
		else
			return -1.0f + tri(invert ? phase - 0.25f : phase - 0.75f);
	}
	float light() {
		return sinf(2.0f*M_PI * phase);
	}
};
//LFO CODE *****************************

struct TremoloStereoFx : Module{
	enum ParamIds {
		WAVE_PARAM,
		FREQ_PARAM,
		BLEND_PARAM,
		INVERT_PARAM,
        BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		SIGNAL_INPUT_L,
		SIGNAL_INPUT_R,
		WAVE_CV_INPUT,
		FREQ_CV_INPUT,
		BLEND_CV_INPUT,
		BYPASS_CV_INPUT,
		RESET_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SIGNAL_OUTPUT_L,
		SIGNAL_OUTPUT_R,
		NUM_OUTPUTS
	};
	enum LightIds {
		WAVE_LIGHT,
		PHASE_POS_LIGHT,
		PHASE_NEG_LIGHT,
		BLEND_LIGHT,
		BYPASS_LED,
		NUM_LIGHTS
	};

	LowFrequencyoscillator oscillatorL, oscillatorR;

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
	float tremolo_signal_L = 0.0f;
	float tremolo_signal_R = 0.0f;
	float blend_control = 0.0f;
	float lfo_modulation_L = 0.0f;
	float lfo_modulation_R = 0.0f;

	TremoloStereoFx() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(TremoloStereoFx::WAVE_PARAM, 0.0f, 1.0f, 0.5f, "Shape", "%", 0.0f, 100.0f);
		configParam(TremoloStereoFx::FREQ_PARAM, 0.0f, 1.0f, 0.5f, "Speed", "%", 0.0f, 100.0f);
		configParam(TremoloStereoFx::BLEND_PARAM, 0.0f, 1.0f, 0.5f, "Blend", "%", 0.0f, 100.0f);

		//New in V2, config switches info without displaying values
		configSwitch(INVERT_PARAM, 0.0f, 1.0f, 0.0f, "Shape Phase Invert", {"ˆ", "ˇ"});

		//New in V2, config temporary buttons info without displaying values
		configButton(BYPASS_SWITCH, "Bypass");
		//new V2, port labels
		//Inputs
		configInput(WAVE_CV_INPUT, "Wave CV");
		configInput(FREQ_CV_INPUT, "Speed CV");
		configInput(BLEND_CV_INPUT, "Blend CV");

		configInput(RESET_CV_INPUT, "Reset");
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

		input_signal_L = clamp(inputs[SIGNAL_INPUT_L].getVoltage(),-10.0f,10.0f);

		if(!inputs[SIGNAL_INPUT_R].isConnected()){
			input_signal_R = input_signal_L;
		}else{
			input_signal_R = clamp(inputs[SIGNAL_INPUT_R].getVoltage(),-10.0f,10.0f);
		}

		float lfo_pitch = clamp(rescale(params[FREQ_PARAM].getValue(), 0.0f, 1.0f, 0.0f, 3.5f) + rescale(inputs[FREQ_CV_INPUT].getVoltage()/10, 0.0f, 1.0f, 0.0f, 3.5f),0.0f, 3.5f);

		//float lfo_pitch = clamp(rescale(params[FREQ_PARAM].getValue(), 0.0f, 1.0f, 0.0f, 3.5f) + inputs[FREQ_CV_INPUT].getVoltage(), 0.0f, 3.5f);
		//LFO L
		oscillatorL.setPitch( lfo_pitch );
		oscillatorL.offset = (0.0f);
		oscillatorL.invert = (params[INVERT_PARAM].getValue() <= 0.0f);
		oscillatorL.setPulseWidth(0.5f);
		oscillatorL.step(1.0f / args.sampleRate);
		oscillatorL.setReset(inputs[RESET_CV_INPUT].getVoltage());
		//LFO R
		oscillatorR.setPitch( lfo_pitch );
		oscillatorR.offset = (0.0f);
		oscillatorR.invert = false;
		oscillatorR.setPulseWidth(0.5f);
		oscillatorR.step(1.0f / args.sampleRate);
		oscillatorR.setReset(inputs[RESET_CV_INPUT].getVoltage());

		float wave = clamp( params[WAVE_PARAM].getValue() + inputs[WAVE_CV_INPUT].getVoltage(), 0.0f, 1.0f );

		float interp_L = crossfade(oscillatorL.sin(), oscillatorL.tri(), wave);
		float interp_R = crossfade(oscillatorR.sin(), oscillatorR.tri(), wave);

		lfo_modulation_L = 5.0f * interp_L;
		lfo_modulation_R = 5.0f * interp_R;

		tremolo_signal_L = input_signal_L * clamp(lfo_modulation_L/10.0f, 0.0f, 1.0f);
		tremolo_signal_R = input_signal_R * clamp(lfo_modulation_R/10.0f, 0.0f, 1.0f);

		blend_control = clamp(params[BLEND_PARAM].getValue() + inputs[BLEND_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);

		output_signal_L = crossfade(input_signal_L,tremolo_signal_L,blend_control);
		output_signal_R = crossfade(input_signal_R,tremolo_signal_R,blend_control);
		//check bypass switch status
		if (fx_bypass){
			fade_in_dry += fade_speed;
			if ( fade_in_dry > 1.0f ) {
				fade_in_dry = 1.0f;
			}
			fade_out_fx -= fade_speed;
			if ( fade_out_fx < 0.0f ) {
				fade_out_fx = 0.0f;
			}
			outputs[SIGNAL_OUTPUT_L].setVoltage(( input_signal_L * fade_in_dry ) + ( output_signal_L * fade_out_fx ));
			outputs[SIGNAL_OUTPUT_R].setVoltage(( input_signal_R * fade_in_dry ) + ( output_signal_R * fade_out_fx ));
		}else{
			fade_in_fx += fade_speed;
			if ( fade_in_fx > 1.0f ) {
				fade_in_fx = 1.0f;
			}
			fade_out_dry -= fade_speed;
			if ( fade_out_dry < 0.0f ) {
				fade_out_dry = 0.0f;
			}
			outputs[SIGNAL_OUTPUT_L].setVoltage(( input_signal_L * fade_out_dry ) + ( output_signal_L * fade_in_fx ));
			outputs[SIGNAL_OUTPUT_R].setVoltage(( input_signal_R * fade_out_dry ) + ( output_signal_R * fade_in_fx ));
		}

		lights[PHASE_POS_LIGHT].setSmoothBrightness(fmaxf(0.0f, oscillatorL.light()), args.sampleTime);
		lights[PHASE_NEG_LIGHT].setSmoothBrightness(fmaxf(0.0f, -oscillatorL.light()), args.sampleTime);
		lights[BLEND_LIGHT].value = clamp(params[BLEND_PARAM].getValue() + inputs[BLEND_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);

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

struct TremoloStereoFxWidget : ModuleWidget { 

	TremoloStereoFxWidget(TremoloStereoFx *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TremoloStereo.svg")));
	
		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//phase switch
		addParam(createParam<as_CKSSwhite>(Vec(13, 100), module, TremoloStereoFx::INVERT_PARAM));
		//KNOBS  
		addParam(createParam<as_FxKnobWhite>(Vec(43, 60), module, TremoloStereoFx::WAVE_PARAM));
		addParam(createParam<as_FxKnobWhite>(Vec(43, 125), module, TremoloStereoFx::FREQ_PARAM));
		addParam(createParam<as_FxKnobWhite>(Vec(43, 190), module, TremoloStereoFx::BLEND_PARAM));
		//LIGHTS
		addChild(createLight<SmallLight<YellowRedLight>>(Vec(39, 122), module, TremoloStereoFx::PHASE_POS_LIGHT));
		addChild(createLight<SmallLight<YellowLight>>(Vec(39, 187), module, TremoloStereoFx::BLEND_LIGHT));
		//CV INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(10, 67), module, TremoloStereoFx::WAVE_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(10, 132), module, TremoloStereoFx::FREQ_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(10, 197), module, TremoloStereoFx::BLEND_CV_INPUT));
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(15, 300), module, TremoloStereoFx::SIGNAL_INPUT_L));
		addInput(createInput<as_PJ301MPort>(Vec(15, 330), module, TremoloStereoFx::SIGNAL_INPUT_R));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(50, 300), module, TremoloStereoFx::SIGNAL_OUTPUT_L));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(50, 330), module, TremoloStereoFx::SIGNAL_OUTPUT_R));
		//RESET CV
		addInput(createInput<as_PJ301MPort>(Vec(6, 259), module, TremoloStereoFx::RESET_CV_INPUT));

		//BYPASS CV INPUT
		addInput(createInput<as_PJ301MPort>(Vec(33.5, 259), module, TremoloStereoFx::BYPASS_CV_INPUT));
		//BYPASS SWITCH
		addParam(createParam<LEDBezel>(Vec(61, 260), module, TremoloStereoFx::BYPASS_SWITCH ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(63.2, 262.2), module, TremoloStereoFx::BYPASS_LED));
	
	}
};


Model *modelTremoloStereoFx = createModel<TremoloStereoFx, TremoloStereoFxWidget>("TremoloStereoFx");