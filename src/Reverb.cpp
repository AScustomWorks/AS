//***********************************************************************************************
//
//Reverb module for VCV Rack by Alfredo Santamaria  - AS - https://github.com/AScustomWorks/AS
//
//Based on code from ML_Modules by martin-lueders https://github.com/martin-lueders/ML_modules
//And code from Freeverb by Jezar at Dreampoint - http://www.dreampoint.co.uk
//
//***********************************************************************************************

#include "AS.hpp"
#include "dsp/digital.hpp"

#include "../freeverb/revmodel.hpp"

struct ReverbFx : Module{
	enum ParamIds {
		DECAY_PARAM,
		DAMP_PARAM,
		BLEND_PARAM,
		BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		SIGNAL_INPUT,
		DECAY_CV_INPUT,
		DAMP_CV_INPUT,
		BLEND_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SIGNAL_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		DECAY_LIGHT,
		DAMP_LIGHT,
		BLEND_LIGHT,
		BYPASS_LED,
		NUM_LIGHTS
	};

	revmodel reverb;
	float roomsize, damp; 

	SchmittTrigger bypass_button_trig;

	bool fx_bypass = false;

	ReverbFx() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {

		float gSampleRate = engineGetSampleRate();
		reverb.init(gSampleRate);
	}

	void step() override;

	void onSampleRateChange() override;

	json_t *toJson()override {
		json_t *rootJm = json_object();

		json_t *statesJ = json_array();
		
			json_t *bypassJ = json_boolean(fx_bypass);
			json_array_append_new(statesJ, bypassJ);
		
		json_object_set_new(rootJm, "as_FxBypass", statesJ);

		return rootJm;
	}

	void fromJson(json_t *rootJm)override {
		json_t *statesJ = json_object_get(rootJm, "as_FxBypass");
		
			json_t *bypassJ = json_array_get(statesJ, 0);

			fx_bypass = !!json_boolean_value(bypassJ);
		
	}
	
};

void ReverbFx::onSampleRateChange() {

	float gSampleRate = engineGetSampleRate();

	reverb.init(gSampleRate);

	reverb.setdamp(damp);
	reverb.setroomsize(roomsize);

};


void ReverbFx::step() {

	if (bypass_button_trig.process(params[BYPASS_SWITCH].value)){
		fx_bypass = !fx_bypass;
	}
    lights[BYPASS_LED].value = fx_bypass ? 1.0f : 0.0f;

	float out1, out2;

	out1 = out2 = 0.0f;

	float old_roomsize = roomsize;
	float old_damp = damp;

	float input_signal = clamp(inputs[SIGNAL_INPUT].value,-10.0f,10.0f);
	//float input_signal = inputs[SIGNAL_INPUT].value;
	roomsize = clamp(params[DECAY_PARAM].value + inputs[DECAY_CV_INPUT].value / 10.0f, 0.0f, 0.88f);
	damp = clamp(params[DAMP_PARAM].value + inputs[DAMP_CV_INPUT].value / 10.0f, 0.0f, 1.0f);

	if( old_damp != damp ) reverb.setdamp(damp);
	if( old_roomsize != roomsize) reverb.setroomsize(roomsize);

	reverb.process(input_signal, out1, out2);

	//check bypass switch status
	if (fx_bypass){
		outputs[SIGNAL_OUTPUT].value = inputs[SIGNAL_INPUT].value;
	}else {
		outputs[SIGNAL_OUTPUT].value = input_signal + out1 * clamp(params[BLEND_PARAM].value + inputs[BLEND_CV_INPUT].value / 10.0f, 0.0f, 1.0f);
		//outputs[SIGNAL_OUTPUT2].value = input_signal + out2 * clamp(params[BLEND_PARAM].value + inputs[BLEND_CV_INPUT].value / 10.0, 0.0, 1.0);
	}

	lights[DECAY_LIGHT].value = clamp(params[DECAY_PARAM].value + inputs[DECAY_CV_INPUT].value / 10.0f, 0.0f, 1.0f);
	lights[DAMP_LIGHT].value = clamp(params[DAMP_PARAM].value + inputs[DAMP_CV_INPUT].value / 10.0f, 0.0f, 1.0f);
	lights[BLEND_LIGHT].value = clamp(params[BLEND_PARAM].value + inputs[BLEND_CV_INPUT].value / 10.0f, 0.0f, 1.0f);

}

struct ReverbFxWidget : ModuleWidget 
{ 
    ReverbFxWidget(ReverbFx *module);
};


ReverbFxWidget::ReverbFxWidget(ReverbFx *module) : ModuleWidget(module) {
	box.size = Vec(15 * 6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/Reverb.svg")));
		addChild(panel);
	}

 	//SCREWS
	addChild(Widget::create<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(Widget::create<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//KNOBS  
	addParam(ParamWidget::create<as_FxKnobWhite>(Vec(43, 60), module, ReverbFx::DECAY_PARAM, 0.0f, 0.9f, 0.5f));
	addParam(ParamWidget::create<as_FxKnobWhite>(Vec(43, 125), module, ReverbFx::DAMP_PARAM, 0.0f, 1.0f, 0.0f));
	addParam(ParamWidget::create<as_FxKnobWhite>(Vec(43, 190), module, ReverbFx::BLEND_PARAM, 0.0f, 1.0f, 0.5f));
	//LIGHTS
	addChild(ModuleLightWidget::create<SmallLight<YellowLight>>(Vec(39, 57), module, ReverbFx::DECAY_LIGHT));
	addChild(ModuleLightWidget::create<SmallLight<YellowLight>>(Vec(39, 122), module, ReverbFx::DAMP_LIGHT));
	addChild(ModuleLightWidget::create<SmallLight<YellowLight>>(Vec(39, 187), module, ReverbFx::BLEND_LIGHT));
    //BYPASS SWITCH
  	addParam(ParamWidget::create<LEDBezel>(Vec(33, 260), module, ReverbFx::BYPASS_SWITCH , 0.0f, 1.0f, 0.0f));
  	addChild(ModuleLightWidget::create<LedLight<RedLight>>(Vec(35.2, 262), module, ReverbFx::BYPASS_LED));
    //INS/OUTS
	addInput(Port::create<as_PJ301MPort>(Vec(10, 310), Port::INPUT, module, ReverbFx::SIGNAL_INPUT));
	addOutput(Port::create<as_PJ301MPort>(Vec(55, 310), Port::OUTPUT, module, ReverbFx::SIGNAL_OUTPUT));
	//CV INPUTS
	addInput(Port::create<as_PJ301MPort>(Vec(10, 67), Port::INPUT, module, ReverbFx::DECAY_CV_INPUT));
	addInput(Port::create<as_PJ301MPort>(Vec(10, 132), Port::INPUT, module, ReverbFx::DAMP_CV_INPUT));
	addInput(Port::create<as_PJ301MPort>(Vec(10, 197), Port::INPUT, module, ReverbFx::BLEND_CV_INPUT));
	
}

Model *modelReverbFx = Model::create<ReverbFx, ReverbFxWidget>("AS", "ReverbFx", "Reverb FX", REVERB_TAG, EFFECT_TAG);