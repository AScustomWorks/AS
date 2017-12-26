//**************************************************************************************
//8 channel mixer VCV Rack mods by Alfredo Santamaria - AS - http://www.hakken.com.mx
//
//Based on DrumsMixer VCV Rack by Autodafe http://www.autodafe.net
//Based on code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"
#include "dsp/digital.hpp"

struct Mixer8ch : Module {
	enum ParamIds {
		MIX_PARAM,
		CH1_PARAM,
		CH2_PARAM,
		CH3_PARAM,
		CH4_PARAM,
		CH5_PARAM,
		CH6_PARAM,
		CH7_PARAM,
		CH8_PARAM, 

		CH1_PAN_PARAM,
		CH2_PAN_PARAM,
		CH3_PAN_PARAM,
		CH4_PAN_PARAM,
		CH5_PAN_PARAM,
		CH6_PAN_PARAM,
		CH7_PAN_PARAM,
		CH8_PAN_PARAM,
 
		CH1MUTE,
		CH2MUTE,
		CH3MUTE,
		CH4MUTE,
		CH5MUTE,
		CH6MUTE,
		CH7MUTE,
		CH8MUTE,
		MASTER_MUTE,

		NUM_PARAMS
	};
	enum InputIds {
		MIX_CV_INPUT,

		CH1_INPUT,
		CH1_CV_INPUT,
		CH1_CV_PAN_INPUT,

		CH2_INPUT,
		CH2_CV_INPUT,
		CH2_CV_PAN_INPUT,

		CH3_INPUT,
		CH3_CV_INPUT,
		CH3_CV_PAN_INPUT,

		CH4_INPUT,
		CH4_CV_INPUT,
		CH4_CV_PAN_INPUT,

		CH5_INPUT,
		CH5_CV_INPUT,
		CH5_CV_PAN_INPUT,

		CH6_INPUT,
		CH6_CV_INPUT,
		CH6_CV_PAN_INPUT,

		CH7_INPUT,
		CH7_CV_INPUT,
		CH7_CV_PAN_INPUT,

		CH8_INPUT,
		CH8_CV_INPUT,
		CH8_CV_PAN_INPUT,		

		NUM_INPUTS
	};
	enum OutputIds {
		MIX_OUTPUTL,
		MIX_OUTPUTR,
		CH1_OUTPUT,
		CH2_OUTPUT,
		CH3_OUTPUT,
		CH4_OUTPUT,
		CH5_OUTPUT,
		CH6_OUTPUT,
		CH7_OUTPUT,
		CH8_OUTPUT,
		
		NUM_OUTPUTS
	};

	enum LightIds {
		MUTE_LIGHT1,
		MUTE_LIGHT2,
		MUTE_LIGHT3,
		MUTE_LIGHT4,
		MUTE_LIGHT5,
		MUTE_LIGHT6,
		MUTE_LIGHT7,
		MUTE_LIGHT8,
		MUTE_LIGHT_MASTER,
		NUM_LIGHTS
	};
	Mixer8ch() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		reset();
	}
	void step() override;

	SchmittTrigger ch1mute;
	SchmittTrigger ch2mute;
	SchmittTrigger ch3mute;
	SchmittTrigger ch4mute;
	SchmittTrigger ch5mute;
	SchmittTrigger ch6mute;
	SchmittTrigger ch7mute;
	SchmittTrigger ch8mute;
	SchmittTrigger mCHmute;

	float ch1m = false;
	float ch2m = false;
	float ch3m = false;
	float ch4m = false;
	float ch5m = false;
	float ch6m = false;
	float ch7m = false;
	float ch8m = false;
	float mCHm = false;

	float ch1MuteLight;
	float ch2MuteLight;
	float ch3MuteLight;
	float ch4MuteLight;
	float ch5MuteLight;
	float ch6MuteLight;
	float ch7MuteLight;
	float ch8MuteLight;
	float mCHMuteLight;

	float ch1L = 0.0;
	float ch1R = 0.0;
	float ch2L = 0.0;
	float ch2R = 0.0;
	float ch3L = 0.0;
	float ch3R = 0.0;
	float ch4L = 0.0;
	float ch4R = 0.0;
	float ch5L = 0.0;
	float ch5R = 0.0;
	float ch6L = 0.0;
	float ch6R = 0.0;
	float ch7L = 0.0;
	float ch7R = 0.0;
	float ch8L = 0.0;
	float ch8R = 0.0;
	float mixL = 0.0;
	float mixR = 0.0;

 	json_t *toJson()override {
		json_t *rootJm = json_object();

		json_t *mutesJ = json_array();
		
			json_t *muteJ1 = json_integer((int) ch1m);
			json_t *muteJ2 = json_integer((int) ch2m);
			json_t *muteJ3 = json_integer((int) ch3m);
			json_t *muteJ4 = json_integer((int) ch4m);
			json_t *muteJ5 = json_integer((int) ch5m);
			json_t *muteJ6 = json_integer((int) ch6m);
			json_t *muteJ7 = json_integer((int) ch7m);
			json_t *muteJ8 = json_integer((int) ch8m);
			json_t *muteJ9 = json_integer((int) mCHm);

			json_array_append_new(mutesJ, muteJ1);
			json_array_append_new(mutesJ, muteJ2);
			json_array_append_new(mutesJ, muteJ3);
			json_array_append_new(mutesJ, muteJ4);
			json_array_append_new(mutesJ, muteJ5);
			json_array_append_new(mutesJ, muteJ6);
			json_array_append_new(mutesJ, muteJ7);
			json_array_append_new(mutesJ, muteJ8);
			json_array_append_new(mutesJ, muteJ9);
		
		json_object_set_new(rootJm, "as_MixerMutes", mutesJ);

		return rootJm;
	}

	void fromJson(json_t *rootJm)override {
		json_t *mutesJ = json_object_get(rootJm, "as_MixerMutes");
		
			json_t *muteJ1 = json_array_get(mutesJ, 0);
			json_t *muteJ2 = json_array_get(mutesJ, 1);
			json_t *muteJ3 = json_array_get(mutesJ, 2);
			json_t *muteJ4 = json_array_get(mutesJ, 3);
			json_t *muteJ5 = json_array_get(mutesJ, 4);
			json_t *muteJ6 = json_array_get(mutesJ, 5);
			json_t *muteJ7 = json_array_get(mutesJ, 6);
			json_t *muteJ8 = json_array_get(mutesJ, 7);
			json_t *muteJ9 = json_array_get(mutesJ, 8);



			ch1m = !!json_integer_value(muteJ1);
			ch2m = !!json_integer_value(muteJ2);
			ch3m = !!json_integer_value(muteJ3);
			ch4m = !!json_integer_value(muteJ4);
			ch5m = !!json_integer_value(muteJ5);
			ch6m = !!json_integer_value(muteJ6);
			ch7m = !!json_integer_value(muteJ7);
			ch8m = !!json_integer_value(muteJ8);
			mCHm = !!json_integer_value(muteJ9);
		
	}
	//PAN LEVEL
	float PanL(float balance, float cv){ // -1...+1
			float p, gl;
			p=M_PI*(balance+1)/4;
			if(cv){
				gl=::cos(p)*(1-((cv+1)/5));
			}else 
				{gl=::cos(p);
			}
			return gl;
	}
	
	float PanR(float balance , float cv){
			float p, gr;
			p=M_PI*(balance+1)/4;
			if(cv){gr=::sin(p)*((cv+1)/5);
			}else{
				gr=::sin(p);
			}
			return gr;
	}
};

void Mixer8ch::step() {
	//MUTE BUTTONS	
	if (ch1mute.process(params[CH1MUTE].value)) {
		ch1m = !ch1m;	
	}
	ch1MuteLight = ch1m ? 1.0 : 0.0;
	lights[MUTE_LIGHT1].value = ch1MuteLight;
	if (ch2mute.process(params[CH2MUTE].value)) {
		ch2m = !ch2m;	
	}
	ch2MuteLight = ch2m ? 1.0 : 0.0;
	lights[MUTE_LIGHT2].value = ch2MuteLight;
	if (ch3mute.process(params[CH3MUTE].value)) {
		ch3m = !ch3m;
	}
	ch3MuteLight = ch3m ? 1.0 : 0.0;
	lights[MUTE_LIGHT3].value = ch3MuteLight;
	if (ch4mute.process(params[CH4MUTE].value)) {
		ch4m = !ch4m;
	}
	ch4MuteLight = ch4m ? 1.0 : 0.0;
	lights[MUTE_LIGHT4].value = ch4MuteLight;
	if (ch5mute.process(params[CH5MUTE].value)) {
		ch5m = !ch5m;	
	}
	ch5MuteLight = ch5m ? 1.0 : 0.0;
	lights[MUTE_LIGHT5].value = ch5MuteLight;
	if (ch6mute.process(params[CH6MUTE].value)) {
		ch6m = !ch6m;
	}
	ch6MuteLight = ch6m ? 1.0 : 0.0;
	lights[MUTE_LIGHT6].value = ch6MuteLight;
	if (ch7mute.process(params[CH7MUTE].value)) {
		ch7m = !ch7m;
	}
	ch7MuteLight = ch7m ? 1.0 : 0.0;
	lights[MUTE_LIGHT7].value = ch7MuteLight;
	if (ch8mute.process(params[CH8MUTE].value)) {
		ch8m = !ch8m;
	}
	ch8MuteLight = ch8m ? 1.0 : 0.0;
	lights[MUTE_LIGHT8].value = ch8MuteLight;

	if (mCHmute.process(params[MASTER_MUTE].value)) {
		mCHm = !mCHm;
	}
	mCHMuteLight = mCHm ? 1.0 : 0.0;
	lights[MUTE_LIGHT_MASTER].value = mCHMuteLight;

	ch1L = (1-ch1m) * (inputs[CH1_INPUT].value) * params[CH1_PARAM].value * PanL(params[CH1_PAN_PARAM].value,(inputs[CH1_CV_PAN_INPUT].value))* clampf(inputs[CH1_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch1R = (1-ch1m) * (inputs[CH1_INPUT].value) * params[CH1_PARAM].value * PanR(params[CH1_PAN_PARAM].value,(inputs[CH1_CV_PAN_INPUT].value)) * clampf(inputs[CH1_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch2L = (1-ch2m) *(inputs[CH2_INPUT].value) * params[CH2_PARAM].value * PanL(params[CH2_PAN_PARAM].value,(inputs[CH2_CV_PAN_INPUT].value)) * clampf(inputs[CH2_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch2R = (1-ch2m) *(inputs[CH2_INPUT].value) * params[CH2_PARAM].value * PanR(params[CH2_PAN_PARAM].value,(inputs[CH2_CV_PAN_INPUT].value)) * clampf(inputs[CH2_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch3L = (1-ch3m) *(inputs[CH3_INPUT].value) * params[CH3_PARAM].value * PanL(params[CH3_PAN_PARAM].value,(inputs[CH3_CV_PAN_INPUT].value)) * clampf(inputs[CH3_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch3R = (1-ch3m) *(inputs[CH3_INPUT].value) * params[CH3_PARAM].value * PanR(params[CH3_PAN_PARAM].value,(inputs[CH3_CV_PAN_INPUT].value)) * clampf(inputs[CH3_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch4L = (1-ch4m) *(inputs[CH4_INPUT].value) * params[CH4_PARAM].value * PanL(params[CH4_PAN_PARAM].value,(inputs[CH4_CV_PAN_INPUT].value)) * clampf(inputs[CH4_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch4R = (1-ch4m) *(inputs[CH4_INPUT].value) * params[CH4_PARAM].value * PanR(params[CH4_PAN_PARAM].value,(inputs[CH4_CV_PAN_INPUT].value)) * clampf(inputs[CH4_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);

	ch5L = (1-ch5m) *(inputs[CH5_INPUT].value) * params[CH5_PARAM].value * PanL(params[CH5_PAN_PARAM].value,(inputs[CH5_CV_PAN_INPUT].value)) * clampf(inputs[CH5_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch5R = (1-ch5m) *(inputs[CH5_INPUT].value) * params[CH5_PARAM].value * PanR(params[CH5_PAN_PARAM].value,(inputs[CH5_CV_PAN_INPUT].value)) * clampf(inputs[CH5_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch6L = (1-ch6m) *(inputs[CH6_INPUT].value) * params[CH6_PARAM].value * PanL(params[CH6_PAN_PARAM].value,(inputs[CH6_CV_PAN_INPUT].value)) * clampf(inputs[CH6_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch6R = (1-ch6m) *(inputs[CH6_INPUT].value) * params[CH6_PARAM].value * PanR(params[CH6_PAN_PARAM].value,(inputs[CH6_CV_PAN_INPUT].value)) * clampf(inputs[CH6_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch7L = (1-ch7m) *(inputs[CH7_INPUT].value) * params[CH7_PARAM].value * PanL(params[CH7_PAN_PARAM].value,(inputs[CH7_CV_PAN_INPUT].value)) * clampf(inputs[CH7_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch7R = (1-ch7m) *(inputs[CH7_INPUT].value) * params[CH7_PARAM].value * PanR(params[CH7_PAN_PARAM].value,(inputs[CH7_CV_PAN_INPUT].value)) * clampf(inputs[CH7_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch8L = (1-ch8m) *(inputs[CH8_INPUT].value) * params[CH8_PARAM].value * PanL(params[CH8_PAN_PARAM].value,(inputs[CH8_CV_PAN_INPUT].value)) * clampf(inputs[CH8_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	ch8R = (1-ch8m) *(inputs[CH8_INPUT].value) * params[CH8_PARAM].value * PanR(params[CH8_PAN_PARAM].value,(inputs[CH8_CV_PAN_INPUT].value)) * clampf(inputs[CH8_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);

	if(!mCHm){
		mixL = (ch1L + ch2L + ch3L +ch4L + ch5L + ch6L + ch7L + ch8L) * params[MIX_PARAM].value * clampf(inputs[MIX_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
		mixR = (ch1R + ch2R + ch3R +ch4R + ch5R + ch6R + ch7R + ch8R) * params[MIX_PARAM].value * clampf(inputs[MIX_CV_INPUT].normalize(10.0) / 10.0, 0.0, 1.0);
	}else{
		mixL = 0.0;
		mixR = 0.0;
	}
	outputs[CH1_OUTPUT].value= ch1L+ch1R; 
	outputs[CH2_OUTPUT].value= ch2L+ch2R;
	outputs[CH3_OUTPUT].value= ch3L+ch3R;
	outputs[CH4_OUTPUT].value= ch4L+ch4R;
	outputs[CH5_OUTPUT].value= ch5L+ch5R;
	outputs[CH6_OUTPUT].value= ch6L+ch6R;
	outputs[CH7_OUTPUT].value= ch7L+ch7R;
	outputs[CH8_OUTPUT].value= ch8L+ch8R;
	//check for MONO OUTPUT
	if(!outputs[MIX_OUTPUTR].active){
		outputs[MIX_OUTPUTL].value= mixL+mixR;
		outputs[MIX_OUTPUTR].value= 0.0;
	}else{
		outputs[MIX_OUTPUTL].value= mixL;
		outputs[MIX_OUTPUTR].value= mixR;
	}

}

Mixer8chWidget::Mixer8chWidget() {
	Mixer8ch *module = new Mixer8ch();
	setModule(module);
	//box.size = Vec(15*37, 380);
	box.size = Vec(RACK_GRID_WIDTH*26, RACK_GRID_HEIGHT);
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/8chMixer.svg")));
		
		addChild(panel);
	} 

	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//PAN KNOBS
 	static const float columnPos[8] = {33,73,113,153, 193, 233, 273, 313};
	static const float panPosY = 180;
	addParam(createParam<as_KnobBlack>(Vec(columnPos[0]-5, panPosY), module, Mixer8ch::CH1_PAN_PARAM, -1.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(columnPos[1]-5, panPosY), module, Mixer8ch::CH2_PAN_PARAM, -1.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(columnPos[2]-5, panPosY), module, Mixer8ch::CH3_PAN_PARAM, -1.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(columnPos[3]-5, panPosY), module, Mixer8ch::CH4_PAN_PARAM, -1.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(columnPos[4]-5, panPosY), module, Mixer8ch::CH5_PAN_PARAM, -1.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(columnPos[5]-5, panPosY), module, Mixer8ch::CH6_PAN_PARAM, -1.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(columnPos[6]-5, panPosY), module, Mixer8ch::CH7_PAN_PARAM, -1.0, 1.0, 0.0));
	addParam(createParam<as_KnobBlack>(Vec(columnPos[7]-5, panPosY), module, Mixer8ch::CH8_PAN_PARAM, -1.0, 1.0, 0.0));
	//VOLUME FADERS
	static const float volPosY = 223; 
	addParam(createParam<as_FaderPot>(Vec(columnPos[0]+2, volPosY), module, Mixer8ch::CH1_PARAM, 0.0, 1.0, 0.8));
	addParam(createParam<as_FaderPot>(Vec(columnPos[1]+2, volPosY), module, Mixer8ch::CH2_PARAM, 0.0, 1.0, 0.8));
	addParam(createParam<as_FaderPot>(Vec(columnPos[2]+2, volPosY), module, Mixer8ch::CH3_PARAM, 0.0, 1.0, 0.8));
	addParam(createParam<as_FaderPot>(Vec(columnPos[3]+2, volPosY), module, Mixer8ch::CH4_PARAM, 0.0, 1.0, 0.8));
	addParam(createParam<as_FaderPot>(Vec(columnPos[4]+2, volPosY), module, Mixer8ch::CH5_PARAM, 0.0, 1.0, 0.8));
	addParam(createParam<as_FaderPot>(Vec(columnPos[5]+2, volPosY), module, Mixer8ch::CH6_PARAM, 0.0, 1.0, 0.8));
	addParam(createParam<as_FaderPot>(Vec(columnPos[6]+2, volPosY), module, Mixer8ch::CH7_PARAM, 0.0, 1.0, 0.8));
	addParam(createParam<as_FaderPot>(Vec(columnPos[7]+2, volPosY), module, Mixer8ch::CH8_PARAM, 0.0, 1.0, 0.8));
	//MUTES
	static const float mutePosY = 310;
	addParam(createParam<LEDBezel>(Vec(columnPos[0]+3, mutePosY), module, Mixer8ch::CH1MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(columnPos[0]+5.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT1));
	addParam(createParam<LEDBezel>(Vec(columnPos[1]+3, mutePosY), module, Mixer8ch::CH2MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(columnPos[1]+5.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT2));
	addParam(createParam<LEDBezel>(Vec(columnPos[2]+3, mutePosY), module, Mixer8ch::CH3MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(columnPos[2]+5.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT3));
	addParam(createParam<LEDBezel>(Vec(columnPos[3]+3, mutePosY), module, Mixer8ch::CH4MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(columnPos[3]+5.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT4));
	addParam(createParam<LEDBezel>(Vec(columnPos[4]+3, mutePosY), module, Mixer8ch::CH5MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(columnPos[4]+5.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT5));
	addParam(createParam<LEDBezel>(Vec(columnPos[5]+3, mutePosY), module, Mixer8ch::CH6MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(columnPos[5]+5.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT6));
	addParam(createParam<LEDBezel>(Vec(columnPos[6]+3, mutePosY), module, Mixer8ch::CH7MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(columnPos[6]+5.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT7));
	addParam(createParam<LEDBezel>(Vec(columnPos[7]+3, mutePosY), module, Mixer8ch::CH8MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(columnPos[7]+5.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT8));

	//PORTS
 	static const float portsY[4] = {60,90,120,150};
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[0], portsY[0]), module, Mixer8ch::CH1_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[0], portsY[1]), module, Mixer8ch::CH1_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[0], portsY[2]), module, Mixer8ch::CH1_CV_PAN_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(columnPos[1], portsY[0]), module, Mixer8ch::CH2_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[1], portsY[1]), module, Mixer8ch::CH2_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[1], portsY[2]), module, Mixer8ch::CH2_CV_PAN_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(columnPos[2], portsY[0]), module, Mixer8ch::CH3_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[2], portsY[1]), module, Mixer8ch::CH3_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[2], portsY[2]), module, Mixer8ch::CH3_CV_PAN_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(columnPos[3], portsY[0]), module, Mixer8ch::CH4_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[3], portsY[1]), module, Mixer8ch::CH4_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[3], portsY[2]), module, Mixer8ch::CH4_CV_PAN_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(columnPos[4], portsY[0]), module, Mixer8ch::CH5_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[4], portsY[1]), module, Mixer8ch::CH5_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[4], portsY[2]), module, Mixer8ch::CH5_CV_PAN_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(columnPos[5], portsY[0]), module, Mixer8ch::CH6_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[5], portsY[1]), module, Mixer8ch::CH6_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[5], portsY[2]), module, Mixer8ch::CH6_CV_PAN_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(columnPos[6], portsY[0]), module, Mixer8ch::CH7_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[6], portsY[1]), module, Mixer8ch::CH7_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[6], portsY[2]), module, Mixer8ch::CH7_CV_PAN_INPUT));

	addInput(createInput<as_PJ301MPort>(Vec(columnPos[7], portsY[0]), module, Mixer8ch::CH8_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[7], portsY[1]), module, Mixer8ch::CH8_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(columnPos[7], portsY[2]), module, Mixer8ch::CH8_CV_PAN_INPUT));

	addOutput(createOutput<as_PJ301MPort>(Vec(columnPos[0], portsY[3]), module, Mixer8ch::CH1_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(columnPos[1], portsY[3]), module, Mixer8ch::CH2_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(columnPos[2], portsY[3]), module, Mixer8ch::CH3_OUTPUT)); 
	addOutput(createOutput<as_PJ301MPort>(Vec(columnPos[3], portsY[3]), module, Mixer8ch::CH4_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(columnPos[4], portsY[3]), module, Mixer8ch::CH5_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(columnPos[5], portsY[3]), module, Mixer8ch::CH6_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(columnPos[6], portsY[3]), module, Mixer8ch::CH7_OUTPUT)); 
	addOutput(createOutput<as_PJ301MPort>(Vec(columnPos[7], portsY[3]), module, Mixer8ch::CH8_OUTPUT));
	//MASTER
	addOutput(createOutput<as_PJ301MPort>(Vec(356, portsY[0]), module, Mixer8ch::MIX_OUTPUTL));
	addOutput(createOutput<as_PJ301MPort>(Vec(356, portsY[1]), module, Mixer8ch::MIX_OUTPUTR));
	addInput(createInput<as_PJ301MPort>(Vec(356, portsY[3]), module, Mixer8ch::MIX_CV_INPUT));
	addParam(createParam<as_FaderPot>(Vec(356, volPosY), module, Mixer8ch::MIX_PARAM, 0.0, 1.0, 0.8));
	addParam(createParam<LEDBezel>(Vec(356, mutePosY), module, Mixer8ch::MASTER_MUTE , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(356+2.2, mutePosY+2), module, Mixer8ch::MUTE_LIGHT_MASTER));
}