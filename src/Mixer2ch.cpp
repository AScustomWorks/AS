//**************************************************************************************
//2 channel mixer module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//Based on DrumsMixer VCV Rack by Autodafe http://www.autodafe.net
//Based on code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************

#include "AS.hpp"
//#include "dsp/digital.hpp"

struct Mixer2ch : Module {
	enum ParamIds {
		MIX_PARAM,
		CH1_PARAM,
		CH2_PARAM,

		CH1_PAN_PARAM,
		CH2_PAN_PARAM,

		CH1MUTE,
		CH2MUTE,

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


		LINK_L,
		LINK_R,
		NUM_INPUTS
	};
	enum OutputIds {
		MIX_OUTPUTL,
		MIX_OUTPUTR,
		CH1_OUTPUT,
		CH2_OUTPUT,

		NUM_OUTPUTS
	};

	enum LightIds {
		MUTE_LIGHT1,
		MUTE_LIGHT2,

		MUTE_LIGHT_MASTER,
		NUM_LIGHTS
	};

	Mixer2ch() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(Mixer2ch::CH1_PAN_PARAM, -1.0f, 1.0f, 0.0f, "CH 1 Pan");
		configParam(Mixer2ch::CH2_PAN_PARAM, -1.0f, 1.0f, 0.0f, "CH 2 Pan");
		configParam(Mixer2ch::CH1_PARAM, 0.0f, 1.0f, 0.8f, "CH 1 Gain", "%", 0.0f,100.0f);
		configParam(Mixer2ch::CH2_PARAM, 0.0f, 1.0f, 0.8f, "CH 2 Gain", "%", 0.0f,100.0f);
		configParam(Mixer2ch::MIX_PARAM, 0.0f, 1.0f, 0.8f, "Mix Gain", "%", 0.0f,100.0f);

		//New in V2, config temporary buttons info without displaying values
		configButton(CH1MUTE, "CH 1 Mute");
		configButton(CH2MUTE, "CH 2 Mute");
		configButton(MASTER_MUTE, "Mix Mute");

		//new V2, port labels

		//inputs
		configInput(MIX_CV_INPUT, "Mix CV");
		configInput(LINK_L, "Mix Left Audio");
		configInput(LINK_R, "Mix Right Audio");

		configInput(CH1_INPUT, "CH 1 Audio");
		configInput(CH2_INPUT, "CH 2 Audio");

		configInput(CH1_CV_INPUT, "CH 1 Gain Mod CV");
		configInput(CH2_CV_INPUT, "CH 2 Gain Mod CV");

		configInput(CH1_CV_PAN_INPUT, "CH 1 Pan Mod CV");
		configInput(CH2_CV_PAN_INPUT, "CH 2 Pan Mod CV");

		//Outputs
		configOutput(MIX_OUTPUTL, "Left Audio");
		configOutput(MIX_OUTPUTR, "Right Audio");

		configOutput(CH1_OUTPUT, "CH 1 Audio");
		configOutput(CH2_OUTPUT, "CH 2 Audio");

	}

	dsp::SchmittTrigger ch1mute;
	dsp::SchmittTrigger ch2mute;

	dsp::SchmittTrigger chMmute;

	float ch1m = false;
	float ch2m = false;

	float chMm = false;

	float mixL = 0.0f;
	float mixR = 0.0f;

	//PAN LEVEL
	float PanL(float balance, float cv) { // -1...+1
		float p, inp;
		inp = balance + cv / 5;
		p = M_PI * (clamp(inp, -1.0f, 1.0f) + 1) / 4;
		return ::cos(p);
	}

	float PanR(float balance , float cv) {
		float p, inp;
		inp = balance + cv / 5;
		p = M_PI * (clamp(inp, -1.0f, 1.0f) + 1) / 4;
		return ::sin(p);
	}

	void process(const ProcessArgs &args) override {
		//MUTE BUTTONS
		if (ch1mute.process(params[CH1MUTE].getValue())) {
			ch1m = !ch1m;
		}
		lights[MUTE_LIGHT1].value = ch1m ? 1.0f : 0.0f;
		if (ch2mute.process(params[CH2MUTE].getValue())) {
			ch2m = !ch2m;
		}
		lights[MUTE_LIGHT2].value = ch2m ? 1.0f : 0.0f;

		if (chMmute.process(params[MASTER_MUTE].getValue())) {
			chMm = !chMm;
		}
		lights[MUTE_LIGHT_MASTER].value = chMm ? 1.0f : 0.0f;
		//CHANNEL RESULTS
		float ch1L =  (1-ch1m) * (inputs[CH1_INPUT].getVoltage()) * params[CH1_PARAM].getValue() * PanL(params[CH1_PAN_PARAM].getValue(),(inputs[CH1_CV_PAN_INPUT].getVoltage()))* clamp(inputs[CH1_CV_INPUT].getNormalVoltage(10.0f) / 10.0f, 0.0f, 1.0f);
		float ch1R =  (1-ch1m) * (inputs[CH1_INPUT].getVoltage()) * params[CH1_PARAM].getValue() * PanR(params[CH1_PAN_PARAM].getValue(),(inputs[CH1_CV_PAN_INPUT].getVoltage())) * clamp(inputs[CH1_CV_INPUT].getNormalVoltage(10.0f) / 10.0f, 0.0f, 1.0f);

		float ch2L = (1-ch2m) *(inputs[CH2_INPUT].getVoltage()) * params[CH2_PARAM].getValue() * PanL(params[CH2_PAN_PARAM].getValue(),(inputs[CH2_CV_PAN_INPUT].getVoltage())) * clamp(inputs[CH2_CV_INPUT].getNormalVoltage(10.0f) / 10.0f, 0.0f, 1.0f);
		float ch2R = (1-ch2m) *(inputs[CH2_INPUT].getVoltage()) * params[CH2_PARAM].getValue() * PanR(params[CH2_PAN_PARAM].getValue(),(inputs[CH2_CV_PAN_INPUT].getVoltage())) * clamp(inputs[CH2_CV_INPUT].getNormalVoltage(10.0f) / 10.0f, 0.0f, 1.0f);


		if(!chMm){
			mixL = (ch1L + ch2L) * params[MIX_PARAM].getValue() * clamp(inputs[MIX_CV_INPUT].getNormalVoltage(10.0f) / 10.0f, 0.0f, 1.0f);
			mixR = (ch1R + ch2R) * params[MIX_PARAM].getValue() * clamp(inputs[MIX_CV_INPUT].getNormalVoltage(10.0f) / 10.0f, 0.0f, 1.0f);
			//CHECK FOR INPUT FROM ANOTHER MIXER
			if(inputs[LINK_L].isConnected() && inputs[LINK_R].isConnected()){
				mixL += inputs[LINK_L].getVoltage();
				mixR += inputs[LINK_R].getVoltage();
			}
		}else{
			mixL = 0.0f;
			mixR = 0.0f;
		}

		outputs[CH1_OUTPUT].value= ch1L+ch1R;
		outputs[CH2_OUTPUT].value= ch2L+ch2R;

		//check for MONO OUTPUT

		if(!outputs[MIX_OUTPUTR].isConnected()){
			outputs[MIX_OUTPUTL].value= mixL+mixR;
			outputs[MIX_OUTPUTR].value= 0.0f;
		}else{
			outputs[MIX_OUTPUTL].value= mixL;
			outputs[MIX_OUTPUTR].value= mixR;
		}

			//outputs[MIX_OUTPUTL].value= mixL;
			//outputs[MIX_OUTPUTR].value= mixR;

	}


 	json_t *dataToJson()override {
		json_t *rootJm = json_object();

		json_t *mutesJ = json_array();

			json_t *muteJ1 = json_integer((int) ch1m);
			json_t *muteJ2 = json_integer((int) ch2m);

			json_t *muteJ3 = json_integer((int) chMm);

			json_array_append_new(mutesJ, muteJ1);
			json_array_append_new(mutesJ, muteJ2);

			json_array_append_new(mutesJ, muteJ3);

		json_object_set_new(rootJm, "as_Mixer2Mutes", mutesJ);

		return rootJm;
	}

	void dataFromJson(json_t *rootJm)override {
		json_t *mutesJ = json_object_get(rootJm, "as_Mixer2Mutes");

			json_t *muteJ1 = json_array_get(mutesJ, 0);
			json_t *muteJ2 = json_array_get(mutesJ, 1);

			json_t *muteJ3 = json_array_get(mutesJ, 2);


			ch1m = !!json_integer_value(muteJ1);
			ch2m = !!json_integer_value(muteJ2);

			chMm = !!json_integer_value(muteJ3);

	}
};


struct Mixer2chWidget : ModuleWidget{

	Mixer2chWidget(Mixer2ch *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/2chMixer.svg")));

		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//PAN KNOBS
		static const float columnPos[8] = {33,73,113,153, 193, 233, 273, 313};
		static const float panPosY = 180;
		addParam(createParam<as_KnobBlack>(Vec(columnPos[0]-5, panPosY), module, Mixer2ch::CH1_PAN_PARAM));
		addParam(createParam<as_KnobBlack>(Vec(columnPos[1]-5, panPosY), module, Mixer2ch::CH2_PAN_PARAM));

		//VOLUME FADERS
		static const float volPosY = 223;
		addParam(createParam<as_FaderPot>(Vec(columnPos[0]+2, volPosY), module, Mixer2ch::CH1_PARAM));
		addParam(createParam<as_FaderPot>(Vec(columnPos[1]+2, volPosY), module, Mixer2ch::CH2_PARAM));

		//MUTES
		static const float mutePosY = 310;
		addParam(createParam<LEDBezel>(Vec(columnPos[0]+3, mutePosY), module, Mixer2ch::CH1MUTE ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(columnPos[0]+5.2, mutePosY+2), module, Mixer2ch::MUTE_LIGHT1));
		addParam(createParam<LEDBezel>(Vec(columnPos[1]+3, mutePosY), module, Mixer2ch::CH2MUTE ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(columnPos[1]+5.2, mutePosY+2), module, Mixer2ch::MUTE_LIGHT2));

		//PORTS
		static const float portsY[4] = {60,90,120,150};
		addInput(createInput<as_PJ301MPort>(Vec(columnPos[0], portsY[0]), module, Mixer2ch::CH1_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(columnPos[0], portsY[1]), module, Mixer2ch::CH1_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(columnPos[0], portsY[2]), module, Mixer2ch::CH1_CV_PAN_INPUT));

		addInput(createInput<as_PJ301MPort>(Vec(columnPos[1], portsY[0]), module, Mixer2ch::CH2_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(columnPos[1], portsY[1]), module, Mixer2ch::CH2_CV_INPUT));
		addInput(createInput<as_PJ301MPort>(Vec(columnPos[1], portsY[2]), module, Mixer2ch::CH2_CV_PAN_INPUT));

		addOutput(createOutput<as_PJ301MPortGold>(Vec(columnPos[0], portsY[3]), module, Mixer2ch::CH1_OUTPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(columnPos[1], portsY[3]), module, Mixer2ch::CH2_OUTPUT));


		//OUTPUT
		static const float mstrX = 130;
		addOutput(createOutput<as_PJ301MPortGold>(Vec(mstrX, portsY[0]), module, Mixer2ch::MIX_OUTPUTL));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(mstrX, portsY[1]), module, Mixer2ch::MIX_OUTPUTR));
		addInput(createInput<as_PJ301MPort>(Vec(mstrX, portsY[3]), module, Mixer2ch::MIX_CV_INPUT));
		addParam(createParam<as_FaderPot>(Vec(mstrX, volPosY), module, Mixer2ch::MIX_PARAM));
		addParam(createParam<LEDBezel>(Vec(mstrX, mutePosY), module, Mixer2ch::MASTER_MUTE ));
		addChild(createLight<LEDBezelLight<RedLight>>(Vec(mstrX+2.2, mutePosY+2), module, Mixer2ch::MUTE_LIGHT_MASTER));

		//LINK
		addInput(createInput<as_PJ301MPort>(Vec(columnPos[0], 30), module, Mixer2ch::LINK_L));
		addInput(createInput<as_PJ301MPort>(Vec(columnPos[1], 30), module, Mixer2ch::LINK_R));

	}
};



Model *modelMixer2ch = createModel<Mixer2ch, Mixer2chWidget>("Mixer2ch");
