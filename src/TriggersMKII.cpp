//**************************************************************************************
//TriggersMKII module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************
#include "AS.hpp"
//#include "dsp/digital.hpp"
#include <sstream>
#include <iomanip>
//#include <iostream>
//#include <cmath>

struct TriggersMKII: Module {
    enum ParamIds {
        LABEL_PARAM_1,
        LABEL_PARAM_2,
        TRIGGER_SWITCH_1,
        MOMENTARY_SWITCH_2,
        NUM_PARAMS
    };
    enum InputIds {
        CV_TRIG_INPUT_1,
        CV_TRIG_INPUT_2,
        NUM_INPUTS
    };
    enum OutputIds {

        TRIGGER_OUT1,
        MOMENTARY_OUT2,

        NUM_OUTPUTS
    };
    enum LightIds {
        RUN_LED,
        TRIGGER_LED_1,
        MOMENTARY_LED_2,
        NUM_LIGHTS
    };

    dsp::SchmittTrigger btnTrigger1;
    dsp::SchmittTrigger extTrigger1;
    dsp::SchmittTrigger btnTrigger2;
    dsp::SchmittTrigger extTrigger2;

    const float lightLambda = 0.075f;
    float resetLight1 = 0.0f;
    float resetLight2 = 0.0f;

    int label_num1 = 0;
    int label_num2 = 0;

    dsp::PulseGenerator triggerPulse1;
    bool trg_pulse1 = false;

    dsp::PulseGenerator triggerPulse2;
    bool trg_pulse2 = false;
 
    TriggersMKII() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(TriggersMKII::LABEL_PARAM_1, 0.0, 36.0, 0.0, "CH 1 Label");
        configParam(TriggersMKII::LABEL_PARAM_2, 0.0, 36.0, 0.0, "CH 2 Label");

        configButton(TRIGGER_SWITCH_1, "Momentary Trigger 1");
        configButton(MOMENTARY_SWITCH_2, "Momentary Trigger 2");

		configInput(CV_TRIG_INPUT_1, "CV external trigger 1");
		configInput(CV_TRIG_INPUT_2, "CV external trigger 2");
		//Outputs
		configOutput(TRIGGER_OUT1, "Trigger 1");
		configOutput(MOMENTARY_OUT2, "Trigger 2");


    }

    void process(const ProcessArgs &args) override {

        label_num1 = roundf(params[LABEL_PARAM_1].getValue());
        label_num2 = roundf(params[LABEL_PARAM_2].getValue());

        //TRIGGER 1
        if (btnTrigger1.process(params[TRIGGER_SWITCH_1].getValue())||extTrigger1.process(inputs[CV_TRIG_INPUT_1].getVoltage())) {
            resetLight1 = 1.0;
            triggerPulse1.trigger(1e-3f);
        }

        trg_pulse1 = triggerPulse1.process(1.0 / args.sampleRate);
        outputs[TRIGGER_OUT1].setVoltage((trg_pulse1 ? 10.0f : 0.0f));

        resetLight1 -= resetLight1 / lightLambda / args.sampleRate;
        lights[TRIGGER_LED_1].value = resetLight1;

        //TRIGGER 2
        if (btnTrigger2.process(params[MOMENTARY_SWITCH_2].getValue())||extTrigger2.process(inputs[CV_TRIG_INPUT_2].getVoltage())) {
            resetLight2 = 1.0;
            triggerPulse2.trigger(1e-3f);
        }
        trg_pulse2 = triggerPulse2.process(1.0 / args.sampleRate);
        outputs[MOMENTARY_OUT2].setVoltage((trg_pulse2 ? 10.0f : 0.0f));

        resetLight2 -= resetLight2 / lightLambda / args.sampleRate;
        lights[MOMENTARY_LED_2].value = resetLight2;
        
    }
 
    
};

static const char *label_values[] = {
    "------",
    "  MUTE",
    "  SOLO",
    " RESET",
	" DRUMS",
    "  KICK",
    " SNARE",
    " HIHAT",
    "  CLAP",
    "  PERC",
	"BASS 1",
	"BASS 2",
	" GTR 1",
	" GTR 2",
	"LEAD 1",
	"LEAD 2",
    " PAD 1",
    " PAD 2",
	"CHORDS",
	"  FX 1",
	"  FX 2",
	" SEQ 1",
	" SEQ 2",
	" MIX 1",
	" MIX 2",    
	" AUX 1",
	" AUX 2",
    "    ON",
    "   OFF",
    " START",
    "  STOP",
    " PAUSE",
    "    UP",
    "  DOWN",
    "  LEFT",
    " RIGHT",
    "   RUN",
};

///////////////////////////////////
struct LabelDisplayWidget : TransparentWidget {

    int *value = NULL;
    std::shared_ptr<Font> font;
    std::string fontPath = asset::plugin(pluginInstance, "res/saxmono.ttf");


    void drawLayer(const DrawArgs& args, int layer) override {
        if (layer != 1){
            return;
        }
        if (!value) {
        return;
        }

        font = APP->window->loadFont(fontPath);
        // text 
        if (font) {
            nvgFontSize(args.vg, 18);
            nvgFontFaceId(args.vg, font->handle);
            nvgTextLetterSpacing(args.vg, 2.0);

            std::stringstream to_display;   
            to_display << std::right  << std::setw(5) << *value;

            Vec textPos = Vec(4.0f, 16.0f); 

            NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);
            nvgFillColor(args.vg, textColor);
            //nvgText(args.vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
            nvgText(args.vg, textPos.x, textPos.y, label_values[*value], NULL);
        }
    }
};
////////////////////////////////////

struct TriggersMKIIWidget : ModuleWidget { 

    TriggersMKIIWidget(TriggersMKII *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TriggersMKII.svg")));
    
        //SCREWS
        addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        static const float led_offset = 6.0;//3.3;
        static const float led_center = 15;
        static const float y_offset = 150;
        //TRIGGER 1
        //LABEL DISPLAY 
        LabelDisplayWidget *display1 = new LabelDisplayWidget();
        display1->box.pos = Vec(6,50);
        display1->box.size = Vec(78, 20);
        if (module) {
            display1->value = &module->label_num1;
        }
        addChild(display1); 	

        //PARAM
        addParam(createParam<as_KnobBlackSnap>(Vec(46, 77), module, TriggersMKII::LABEL_PARAM_1));
        //SWITCH
/*         addParam(createParam<BigLEDBezel>(Vec(led_center, 132), module, TriggersMKII::TRIGGER_SWITCH_1));
        addChild(createLight<GiantLight<RedLight>>(Vec(led_center+led_offset, 132+led_offset), module, TriggersMKII::TRIGGER_LED_1)); */

        addParam(createParam<JumboLEDBezel60>(Vec(led_center, 132), module, TriggersMKII::TRIGGER_SWITCH_1));
        addChild(createLight<JumboLedLight60<RedLight>>(Vec(led_center+led_offset, 132+led_offset), module, TriggersMKII::TRIGGER_LED_1));
        //PORTS
        addOutput(createOutput<as_PJ301MPortGold>(Vec(7, 78), module, TriggersMKII::TRIGGER_OUT1));
        addInput(createInput<as_PJ301MPort>(Vec(7, 104), module, TriggersMKII::CV_TRIG_INPUT_1));

        //TRIGGER 2
        //LABEL DISPLAY 
        LabelDisplayWidget *display2 = new LabelDisplayWidget();
        display2->box.pos = Vec(6,50+y_offset);
        display2->box.size = Vec(78, 20);
        if (module) {
            display2->value = &module->label_num2;
        }
        addChild(display2); 	

        //PARAM
        addParam(createParam<as_KnobBlackSnap>(Vec(46, 77+y_offset), module, TriggersMKII::LABEL_PARAM_2));
        //SWITCH
/*         addParam(createParam<BigLEDBezel>(Vec(led_center, 132+y_offset), module, TriggersMKII::MOMENTARY_SWITCH_2));
        addChild(createLight<GiantLight<RedLight>>(Vec(led_center+led_offset, 132+led_offset+y_offset), module, TriggersMKII::MOMENTARY_LED_2)); */
        addParam(createParam<JumboLEDBezel60>(Vec(led_center, 132+y_offset), module, TriggersMKII::MOMENTARY_SWITCH_2));
        addChild(createLight<JumboLedLight60<RedLight>>(Vec(led_center+led_offset, 132+led_offset+y_offset), module, TriggersMKII::MOMENTARY_LED_2));
        //PORTS
        addOutput(createOutput<as_PJ301MPortGold>(Vec(7, 78+y_offset), module, TriggersMKII::MOMENTARY_OUT2));
        addInput(createInput<as_PJ301MPort>(Vec(7, 104+y_offset), module, TriggersMKII::CV_TRIG_INPUT_2));
        
    }
};


Model *modelTriggersMKII = createModel<TriggersMKII, TriggersMKIIWidget>("TriggersMKII");
