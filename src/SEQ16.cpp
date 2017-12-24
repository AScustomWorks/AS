//**************************************************************************************
//16 channel sequencer VCV Rack mods by Alfredo Santamaria - AS - http://www.hakken.com.mx
//
//Based on SEQ16 VCV Rack by Autodafe http://www.autodafe.net
//Based on code taken from the Fundamentals plugins by Andrew Belt http://www.vcvrack.com
//**************************************************************************************
#include "AS.hpp"
#include "dsp/digital.hpp"

struct SEQ16 : Module {
	enum ParamIds {
		CLOCK_PARAM,
		RUN_PARAM,
		RESET_PARAM,
		STEPS_PARAM,
		ROW1_PARAM,
		ROW2_PARAM = ROW1_PARAM + 16,
		ROW3_PARAM = ROW2_PARAM + 16,
		GATE_PARAM = ROW3_PARAM + 16,
		NUM_PARAMS = GATE_PARAM + 16
	};
	enum InputIds {
		CLOCK_INPUT,
		EXT_CLOCK_INPUT,
		RESET_INPUT,
		STEPS_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GATES_OUTPUT,
		ROW1_OUTPUT,
		ROW2_OUTPUT,
		ROW3_OUTPUT,
		GATE_OUTPUT,
		NUM_OUTPUTS = GATE_OUTPUT + 16
	};
	enum LightIds {
		RUNNING_LIGHT,
		RESET_LIGHT,
		GATES_LIGHT,
		ROW_LIGHTS,
		GATE_LIGHTS = ROW_LIGHTS + 3,
		NUM_LIGHTS = GATE_LIGHTS + 16
	};

	bool running = true;
	// for external clock
	SchmittTrigger clockTrigger;
	// For buttons
	SchmittTrigger runningTrigger;
	SchmittTrigger resetTrigger;
	SchmittTrigger gateTriggers[16];
	float phase = 0.0;
	int index = 0;
	bool gateState[16] = {};
	float resetLight = 0.0;
	float stepLights[16] = {};

	enum GateMode {
		TRIGGER,
		RETRIGGER,
		CONTINUOUS,
	};
	GateMode gateMode = TRIGGER;
	PulseGenerator gatePulse;

	SEQ16() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		reset();
	}
	void step() override;

	int numSteps;

	json_t *toJson() override {
		json_t *rootJ = json_object();

		// running
		json_object_set_new(rootJ, "running", json_boolean(running));

		// gates
		json_t *gatesJ = json_array();
		for (int i = 0; i < 16; i++) {
			json_t *gateJ = json_integer((int) gateState[i]);
			json_array_append_new(gatesJ, gateJ);
		}
		json_object_set_new(rootJ, "gates", gatesJ);
		// gateMode
		json_t *gateModeJ = json_integer((int) gateMode);
		json_object_set_new(rootJ, "gateMode", gateModeJ);
		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		// running
		json_t *runningJ = json_object_get(rootJ, "running");
		if (runningJ)
			running = json_is_true(runningJ);
		// gates
		json_t *gatesJ = json_object_get(rootJ, "gates");
		if (gatesJ) {
			for (int i = 0; i < 16; i++) {
				json_t *gateJ = json_array_get(gatesJ, i);
				if (gateJ)
					gateState[i] = !!json_integer_value(gateJ);
			}
		}

		// gateMode
		json_t *gateModeJ = json_object_get(rootJ, "gateMode");
		if (gateModeJ)
			gateMode = (GateMode)json_integer_value(gateModeJ);
	}

	void reset() override {
		for (int i = 0; i < 16; i++) {
			gateState[i] = true;
		}
	}

	void randomize() override {
		for (int i = 0; i < 16; i++) {
			gateState[i] = (randomf() > 0.5);
		}
	}
};


void SEQ16::step() {
	numSteps = roundf(clampf(params[STEPS_PARAM].value, 1.0, 16.0)); 

	const float lightLambda = 0.075;
	// Run
	if (runningTrigger.process(params[RUN_PARAM].value)) {
		running = !running;
	}
	lights[RUNNING_LIGHT].value = running ? 1.0 : 0.0;

	bool nextStep = false;

	if (running) {
		if (inputs[EXT_CLOCK_INPUT].active) {
			// External clock
			if (clockTrigger.process(inputs[EXT_CLOCK_INPUT].value)) {
				phase = 0.0;
				nextStep = true;
			}
		}
		else {
			// Internal clock
			float clockTime = powf(2.0, params[CLOCK_PARAM].value + inputs[CLOCK_INPUT].value);
			phase += clockTime / engineGetSampleRate();
			if (phase >= 1.0) {
				phase -= 1.0;
				nextStep = true;
			}
		}
	}

	// Reset
	if (resetTrigger.process(params[RESET_PARAM].value + inputs[RESET_INPUT].value)) {
		phase = 0.0;
		index = 16;
		nextStep = true;
		resetLight = 1.0;
	}

	if (nextStep) {
		// Advance step
		int numSteps = clampi(roundf(params[STEPS_PARAM].value + inputs[STEPS_INPUT].value), 1, 16);
		index += 1;
		if (index >= numSteps) {
			index = 0;
		}
		stepLights[index] = 1.0;
		gatePulse.trigger(1e-3);
	}

	resetLight -= resetLight / lightLambda / engineGetSampleRate();

	bool pulse = gatePulse.process(1.0 / engineGetSampleRate());

	// Gate buttons
	for (int i = 0; i < 16; i++) {
		if (gateTriggers[i].process(params[GATE_PARAM + i].value)) {
			gateState[i] = !gateState[i];
		}
		bool gateOn = (running && i == index && gateState[i]);
		if (gateMode == TRIGGER)
			gateOn = gateOn && pulse;
		else if (gateMode == RETRIGGER)
			gateOn = gateOn && !pulse;

		outputs[GATE_OUTPUT + i].value = gateOn ? 10.0 : 0.0;
		stepLights[i] -= stepLights[i] / lightLambda / engineGetSampleRate();
		lights[GATE_LIGHTS + i].value = gateState[i] ? 1.0 - stepLights[i] : stepLights[i];
	}

	// Rows
	float row1 = params[ROW1_PARAM + index].value;
	float row2 = params[ROW2_PARAM + index].value;
	float row3 = params[ROW3_PARAM + index].value;
	bool gatesOn = (running && gateState[index]);
	if (gateMode == TRIGGER)
		gatesOn = gatesOn && pulse;
	else if (gateMode == RETRIGGER)
		gatesOn = gatesOn && !pulse;

	// Outputs
	outputs[ROW1_OUTPUT].value = row1;
	outputs[ROW2_OUTPUT].value = row2;
	outputs[ROW3_OUTPUT].value = row3;
	outputs[GATES_OUTPUT].value = gatesOn ? 10.0 : 0.0;
	lights[RESET_LIGHT].value = resetLight;
	lights[GATES_LIGHT].value = gatesOn ? 1.0 : 0.0;
	lights[ROW_LIGHTS].value = row1;
	lights[ROW_LIGHTS + 1].value = row2;
	lights[ROW_LIGHTS + 2].value = row3;
}

struct StepsDisplayWidget : TransparentWidget {

  int *value;
  std::shared_ptr<Font> font;

  StepsDisplayWidget() {
    font = Font::load(assetPlugin(plugin, "res/Segment7Standard.ttf"));
  };

  void draw(NVGcontext *vg) {
    // Background
    NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(vg, backgroundColor);
    nvgFill(vg);
    nvgStrokeWidth(vg, 1.0);
    nvgStrokeColor(vg, borderColor);
    nvgStroke(vg);

    nvgFontSize(vg, 22);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 2.5);

    char displayStr[3];

    sprintf(displayStr, "%2u", (unsigned) *value);

    Vec textPos = Vec(6, 23);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "\\\\", NULL);

    textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(vg, textColor);
    nvgText(vg, textPos.x, textPos.y, displayStr,  NULL);
  }
};

template <typename BASE>
struct MuteLight : BASE {
	MuteLight() {
	  //this->box.size = Vec(20.0, 20.0);
	  this->box.size = mm2px(Vec(6.0, 6.0));
	}
};

SEQ16Widget::SEQ16Widget() {
	SEQ16 *module = new SEQ16();
	setModule(module);
	box.size = Vec(44 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/SEQ16.svg")));
		addChild(panel);
	}
	//LCD screen
		StepsDisplayWidget *display = new StepsDisplayWidget();
	display->box.pos = Vec(351,60);
	display->box.size = Vec(40, 30);
	display->value = &module->numSteps;
	addChild(display);

	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//
	static const float portX[16] = {20,60,100,140,180,220,260,300,340,380,420,460,500,540,580,620};
	static const float elements_offst = 8;
	static const float main_lds_y = 64.4;
	//CLOCK KNOB
	addParam(createParam<as_KnobBlack>(Vec(portX[1]-elements_offst, 56), module, SEQ16::CLOCK_PARAM, -2.0, 6.0, 2.0));
	//RUN RESET SWITCHES & LEDS
	addParam(createParam<LEDBezel>(Vec(portX[3], main_lds_y), module, SEQ16::RUN_PARAM , 0.0, 1.0, 0.0));
	addChild(createLight<MuteLight<RedLight>>(Vec(portX[3]+2.2, main_lds_y+2), module, SEQ16::RUNNING_LIGHT));
	addParam(createParam<LEDBezel>(Vec(portX[5], main_lds_y), module, SEQ16::RESET_PARAM , 0.0, 1.0, 0.0));
	addChild(createLight<MuteLight<RedLight>>(Vec(portX[5]+2.2, main_lds_y+2), module, SEQ16::RESET_LIGHT));
	//STEPS KNOBS
	addParam(createParam<as_KnobBlack>(Vec(portX[7]-elements_offst, 56), module, SEQ16::STEPS_PARAM, 1.0, 16.0, 16.0));
	static const float main_inputs_offst = 1;
	static const float main_inputs_y = 98;
	//SEQ VC INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(portX[1]- main_inputs_offst, main_inputs_y), module, SEQ16::CLOCK_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(portX[3]-main_inputs_offst, main_inputs_y), module, SEQ16::EXT_CLOCK_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(portX[5]-main_inputs_offst, main_inputs_y), module, SEQ16::RESET_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(portX[7]-main_inputs_offst, main_inputs_y), module, SEQ16::STEPS_INPUT));
	//GATE/ROW LEDS
	addChild(createLight<MediumLight<RedLight>>(Vec(portX[11]+elements_offst, main_lds_y), module, SEQ16::GATES_LIGHT));
	addChild(createLight<MediumLight<RedLight>>(Vec(portX[12]+elements_offst, main_lds_y), module, SEQ16::ROW_LIGHTS));
	addChild(createLight<MediumLight<RedLight>>(Vec(portX[13]+elements_offst, main_lds_y), module, SEQ16::ROW_LIGHTS + 1));
	addChild(createLight<MediumLight<RedLight>>(Vec(portX[14]+elements_offst, main_lds_y), module, SEQ16::ROW_LIGHTS + 2));
	//GATE/ROW OUTPUTS
	addOutput(createOutput<as_PJ301MPort>(Vec(portX[11], 98), module, SEQ16::GATES_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(portX[12], 98), module, SEQ16::ROW1_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(portX[13], 98), module, SEQ16::ROW2_OUTPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(portX[14], 98), module, SEQ16::ROW3_OUTPUT));

	for (int i = 0; i < 16; i++) {
		//ROW KNOBS
		addParam(createParam<as_KnobBlack>(Vec(portX[i]-elements_offst, 157), module, SEQ16::ROW1_PARAM + i, 0.0, 10.0, 0.0));
		addParam(createParam<as_KnobBlack>(Vec(portX[i]-elements_offst, 198), module, SEQ16::ROW2_PARAM + i, 0.0, 10.0, 0.0));
		addParam(createParam<as_KnobBlack>(Vec(portX[i]-elements_offst, 240), module, SEQ16::ROW3_PARAM + i, 0.0, 10.0, 0.0));
		//GATE LEDS
		addParam(createParam<LEDButton>(Vec(portX[i]+1.5, 284), module, SEQ16::GATE_PARAM + i, 0.0, 1.0, 0.0));
		addChild(createLight<MediumLight<RedLight>>(Vec(portX[i]+5.7, 287.9), module, SEQ16::GATE_LIGHTS + i));
		//GATE STEPS OUT
		addOutput(createOutput<as_PJ301MPort>(Vec(portX[i]-2, 310), module, SEQ16::GATE_OUTPUT + i));
	}
}

struct SEQ16GateModeItem : MenuItem {
	SEQ16 *seq16;
	SEQ16::GateMode gateMode;
	void onAction(EventAction &e) override {
		seq16->gateMode = gateMode;
	}
	void step() override {
		rightText = (seq16->gateMode == gateMode) ? "✔" : "";
	}
};

Menu *SEQ16Widget::createContextMenu() {
	Menu *menu = ModuleWidget::createContextMenu();

	MenuLabel *spacerLabel = new MenuLabel();
	menu->addChild(spacerLabel);

	SEQ16 *seq16 = dynamic_cast<SEQ16*>(module);
	assert(seq16);

	MenuLabel *modeLabel = new MenuLabel();
	modeLabel->text = "Gate Mode";
	menu->addChild(modeLabel);

	SEQ16GateModeItem *triggerItem = new SEQ16GateModeItem();
	triggerItem->text = "Trigger";
	triggerItem->seq16 = seq16;
	triggerItem->gateMode = SEQ16::TRIGGER;
	menu->addChild(triggerItem);

	SEQ16GateModeItem *retriggerItem = new SEQ16GateModeItem();
	retriggerItem->text = "Retrigger";
	retriggerItem->seq16 = seq16;
	retriggerItem->gateMode = SEQ16::RETRIGGER;
	menu->addChild(retriggerItem);

	SEQ16GateModeItem *continuousItem = new SEQ16GateModeItem();
	continuousItem->text = "Continuous";
	continuousItem->seq16 = seq16;
	continuousItem->gateMode = SEQ16::CONTINUOUS;
	menu->addChild(continuousItem);

	return menu;
}
