//**************************************************************************************
//TriggersMKIMKI  module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************
#include "AS.hpp"
//#include "dsp/digital.hpp"
#include <sstream>
#include <iomanip>

struct TriggersMKI: Module {
    enum ParamIds {
        VOLTAGE_PARAM,
        RUN_SWITCH,
        MOMENTARY_SWITCH,
        NUM_PARAMS
    };
    enum InputIds {
        CV_RUN_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        TRIGGER_OUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        RUN_LED,
        MOMENTARY_LED,
        NUM_LIGHTS
    };

    dsp::SchmittTrigger LatchTrigger;
    dsp::SchmittTrigger LatchExtTrigger;
    dsp::SchmittTrigger BtnTrigger;
    dsp::SchmittTrigger BtnExtTrigger;

    const float lightLambda = 0.075;
    float resetLight = 0.0f;
    float volts = 0.0f;
    bool running = false;
    float display_volts = 0.0f;
    bool negative_volts = false;

    dsp::PulseGenerator triggerPulse;
    bool trg_pulse = false;

    TriggersMKI() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(TriggersMKI::VOLTAGE_PARAM, -10.0f, 10.0f, 0.0f, "Volts", " V");
        configParam(TriggersMKI::RUN_SWITCH, 0.0, 1.0, 0.0, "Latch Switch");
        configParam(TriggersMKI::MOMENTARY_SWITCH, 0.0, 1.0, 0.0, "Momentary Switch");

        configButton(RUN_SWITCH, "Latch Switch");
        configButton(MOMENTARY_SWITCH, "Momentary Trigger");

		configInput(CV_RUN_INPUT, "CV external trigger");
		//Outputs
		configOutput(TRIGGER_OUT, "Trigger");

    }

    void process(const ProcessArgs &args) override{

        display_volts = 0.0f;

        volts = params[VOLTAGE_PARAM].getValue();
        display_volts = volts;
        negative_volts = false;
        if(volts< 0.0){
            negative_volts = true;
        }
        if(negative_volts){
            display_volts = - display_volts;
            //doesn't update fast enough to get rid of the negative 0 display color
            /*
            if(display_volts == -0.0){
                display_volts = 0.0;
            }
            */
        }else{
            display_volts = volts;
        }
        //LATCH TRIGGER
        //EXTERNAL TRIGGER
        if (LatchTrigger.process(params[RUN_SWITCH].getValue())||LatchExtTrigger.process(inputs[CV_RUN_INPUT].getVoltage())) {
            running = !running;
        }
        //INTERNAL TRIGGER
        if (running) {
            lights[RUN_LED].value = 1.0f;
            outputs[TRIGGER_OUT].setVoltage(volts);
        }else{
            lights[RUN_LED].value = 0.0f;
            outputs[TRIGGER_OUT].setVoltage(0.0f);
        }

        //MOMENTARY TRIGGER
        //updated to use pulses
        if (BtnTrigger.process(params[MOMENTARY_SWITCH].getValue())) {
            resetLight = 1.0;
            if (!running) {
                triggerPulse.trigger(1e-3f);

            }
        }
        if(!running){
            trg_pulse = triggerPulse.process(1.0 / args.sampleRate);
            outputs[TRIGGER_OUT].setVoltage((trg_pulse ? volts : 0.0f));
        }


        resetLight -= resetLight / lightLambda / args.sampleRate;
        lights[MOMENTARY_LED].value = resetLight;

    }


    json_t *dataToJson() override
    {
        json_t *rootJ = json_object();
        json_t *button_statesJ = json_array();
        json_t *button_stateJ = json_integer((int)running);
        json_array_append_new(button_statesJ, button_stateJ);		
        json_object_set_new(rootJ, "run", button_statesJ);    
        return rootJ;
    }

    void dataFromJson(json_t *rootJ) override
    {
        json_t *button_statesJ = json_object_get(rootJ, "run");
        if (button_statesJ)
        {			
                json_t *button_stateJ = json_array_get(button_statesJ,0);
                if (button_stateJ)
                    running = !!json_integer_value(button_stateJ);			
        }  
    }  
    
};

///////////////////////////////////
struct VoltsDisplayWidget : TransparentWidget {

    float *value = NULL;
    bool *negative;
    std::shared_ptr<Font> font;
    std::string fontPath = asset::plugin(pluginInstance, "res/Segment7Standard.ttf");


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
            nvgTextLetterSpacing(args.vg, 2.5);

            char display_string[10];
            sprintf(display_string,"%5.2f",*value);

            Vec textPos = Vec(3.0f, 17.0f); 

            NVGcolor textColor = nvgRGB(0xf0, 0x00, 0x00);

            if(*negative){
                textColor = nvgRGB(0xf0, 0x00, 0x00);
            }else{
                //textColor = nvgRGB(0x90, 0xc6, 0x3e);
                textColor = nvgRGB(0x00, 0xaf, 0x25);
            }
            
            nvgFillColor(args.vg, textColor);
            nvgText(args.vg, textPos.x, textPos.y, display_string, NULL);
        }

  }
};
////////////////////////////////////

struct TriggersMKIWidget : ModuleWidget { 

    TriggersMKIWidget(TriggersMKI *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TriggersMKI.svg")));
    
        //SCREWS
        addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        //VOLTS DISPLAY 
        VoltsDisplayWidget *display1 = new VoltsDisplayWidget();
        display1->box.pos = Vec(10,50);
        display1->box.size = Vec(70, 20);
        if (module) {
            display1->value = &module->display_volts;
            display1->negative = &module->negative_volts;
        }
        addChild(display1); 

        //PARAMS
        addParam(createParam<as_KnobBlack>(Vec(26, 77), module, TriggersMKI::VOLTAGE_PARAM));
        //SWITCHES
        static const float led_offset = 6.0;//3.3;
        static const float led_center = 15;
/*         addParam(createParam<BigLEDBezel>(Vec(led_center, 182), module, TriggersMKI::RUN_SWITCH));
        addChild(createLight<GiantLight<RedLight>>(Vec(led_center+led_offset, 182+led_offset), module, TriggersMKI::RUN_LED)); */

        addParam(createParam<JumboLEDBezel60>(Vec(led_center, 182), module, TriggersMKI::RUN_SWITCH));
        addChild(createLight<JumboLedLight60<RedLight>>(Vec(led_center+led_offset, 182+led_offset), module, TriggersMKI::RUN_LED));

/*         addParam(createParam<BigLEDBezel>(Vec(led_center, 262), module, TriggersMKI::MOMENTARY_SWITCH));
        addChild(createLight<GiantLight<RedLight>>(Vec(led_center+led_offset, 262+led_offset), module, TriggersMKI::MOMENTARY_LED)); */

        addParam(createParam<JumboLEDBezel60>(Vec(led_center, 262), module, TriggersMKI::MOMENTARY_SWITCH));
        addChild(createLight<JumboLedLight60<RedLight>>(Vec(led_center+led_offset, 262+led_offset), module, TriggersMKI::MOMENTARY_LED));

        //PORTS
        addInput(createInput<as_PJ301MPort>(Vec(10, 145), module, TriggersMKI::CV_RUN_INPUT));
        addOutput(createOutput<as_PJ301MPortGold>(Vec(55, 145), module, TriggersMKI::TRIGGER_OUT));

    }
};

Model *modelTriggersMKI = createModel<TriggersMKI, TriggersMKIWidget>("TriggersMKI");