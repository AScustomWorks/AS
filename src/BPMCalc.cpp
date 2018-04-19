//**************************************************************************************
//
//BPM Calculator module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************

#include "AS.hpp"

#include <sstream>
#include <iomanip>


struct BPMCalc : Module {
	enum ParamIds {    
    TEMPO_PARAM,       
		NUM_PARAMS
	};  
	enum InputIds {  
		NUM_INPUTS
	};
	enum OutputIds {     
		NUM_OUTPUTS
	};
  enum LightIds {
		NUM_LIGHTS
	};

  float bpm = 120;
  float millisecs = 60000;
  float mult = 1000;
  float millisecondsPerBeat;
  float millisecondsPerMeasure;
  float bar = 1;

  float secondsPerBeat = 0.0f;
	float secondsPerMeasure = 0.0f;
  //ms variables
  float half_note_d = 1.0f;
  float half_note = 1.0f;
  float half_note_t =1.0f;

  float qt_note_d = 1.0f;
  float qt_note = 1.0f;
  float qt_note_t = 1.0f;

  float eight_note_d = 1.0f;
  float eight_note =1.0f;
  float eight_note_t = 1.0f;

  float sixth_note_d =1.0f;
  float sixth_note = 1.0f;
  float sixth_note_t = 1.0f;

  float trth_note_d = 1.0f;
  float trth_note = 1.0f;
  float trth_note_t = 1.0f;
  //hz variables
  float hz_bar = 1.0f;
  float half_hz_d = 1.0f;
  float half_hz = 1.0f;
  float half_hz_t = 1.0f;

  float qt_hz_d = 1.0f;
  float qt_hz = 1.0f;
  float qt_hz_t = 1.0f;

  float eight_hz_d = 1.0f;
  float eight_hz = 1.0f;
  float eight_hz_t = 1.0f;

  float sixth_hz_d = 1.0f;
  float sixth_hz = 1.0f;
  float sixth_hz_t = 1.0f;

  float trth_hz_d = 1.0f;
  float trth_hz = 1.0f;
  float trth_hz_t = 1.0f;
  
	BPMCalc() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}

	void step() override;


};

void BPMCalc::step() {
  bpm = params[TEMPO_PARAM].value;
  if (bpm<30){
    bpm = 30;
  }
  bpm = std::round(bpm);
  millisecondsPerBeat = millisecs/bpm;
	millisecondsPerMeasure = millisecondsPerBeat * 4;

  secondsPerBeat = 60 / bpm;
	secondsPerMeasure = secondsPerBeat * 4;

  bar = (millisecondsPerMeasure);

  half_note_d = ( millisecondsPerBeat * 3 );
  half_note = ( millisecondsPerBeat * 2 );
  half_note_t = ( millisecondsPerBeat * 2 * 2 / 3 );

  qt_note_d = ( millisecondsPerBeat / 2 ) * 3;
  qt_note = millisecondsPerBeat;
  qt_note_t = ( millisecondsPerBeat * 2 ) / 3;

  eight_note_d = ( millisecondsPerBeat / 4 ) * 3;
  eight_note = millisecondsPerBeat / 2;
  eight_note_t = millisecondsPerBeat / 3;

  sixth_note_d = ( millisecondsPerBeat / 4 ) * 1.5;
  sixth_note = millisecondsPerBeat / 4;
  sixth_note_t = millisecondsPerBeat / 6;

  trth_note_d = ( millisecondsPerBeat / 8 ) * 1.5;
  trth_note = millisecondsPerBeat / 8;
  trth_note_t = millisecondsPerBeat / 8 * 2 / 3;
  //hz measures
  hz_bar = (1/secondsPerMeasure);

  half_hz_d = mult / half_note_d;
  half_hz = mult / half_note;
  half_hz_t = mult / half_note_t;

  qt_hz_d = mult / qt_note_d;
  qt_hz = mult / qt_note;
  qt_hz_t = mult / qt_note_t;

  eight_hz_d = mult / eight_note_d;
  eight_hz = mult / eight_note;
  eight_hz_t = mult / eight_note_t;

  sixth_hz_d = mult / sixth_note_d;
  sixth_hz = mult / sixth_note;
  sixth_hz_t = mult / sixth_note_t;

  trth_hz_d = mult / trth_note_d;
  trth_hz = mult / trth_note;
  trth_hz_t = mult / trth_note_t;


  //seems like round calcs are not really needed:
  /*
  half_note_d = std::round(millisecondsPerBeat * 3 * mult)/mult;
  half_note = std::round(millisecondsPerBeat * 2 * mult)/mult;
  half_note_t = std::round(millisecondsPerBeat * 2 * 2 / 3 * mult)/mult;
  */
}

////////////////////////////////////
struct BpmDisplayWidget : TransparentWidget {
  float *value;
  std::shared_ptr<Font> font;

  BpmDisplayWidget() {
    font = Font::load(assetPlugin(plugin, "res/Segment7Standard.ttf"));
  };

  void draw(NVGcontext *vg) override
  {
    // Background
    //NVGcolor backgroundColor = nvgRGB(0x20, 0x20, 0x20);
    NVGcolor backgroundColor = nvgRGB(0x20, 0x10, 0x10);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(vg, backgroundColor);
    nvgFill(vg);
    nvgStrokeWidth(vg, 1.5);
    nvgStrokeColor(vg, borderColor);
    nvgStroke(vg);    
    // text 
    nvgFontSize(vg, 18);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 2.5);

    std::stringstream to_display;   
    to_display << std::setw(3) << *value;

    Vec textPos = Vec(4.0f, 17.0f); 

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "\\\\", NULL);

    textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(vg, textColor);
    nvgText(vg, textPos.x, textPos.y, to_display.str().c_str(), NULL);
  }
};


///////////////////////////////////

struct TxtDisplay : TransparentWidget{
  BPMCalc *module;
  int frame = 0;
  const int v_s = 14;
  const int h_off = 60;
   const int hz_txt_off = 125;
  std::shared_ptr<Font> font;


  TxtDisplay(){
    font = Font::load(assetPlugin(plugin, "res/saxmono.ttf"));
  }

  void drawMessage(NVGcontext *vg, Vec pos){
    nvgFontSize(vg, 13);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 0);
    nvgTextAlign(vg, NVG_ALIGN_LEFT);
    NVGcolor  textColor = nvgRGB(0xf0, 0x00, 0x00);
    nvgFillColor(vg, textColor);
    //note texts
    nvgText(vg, pos.x, pos.y+v_s * 0,  "     1:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 1,  "  •1/2:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 2,  "   1/2:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 3,  "  t1/2:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 4,  "  •1/4:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 5,  "   1/4:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 6,  "  t1/4:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 7,  "  •1/8:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 8,  "   1/8:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 9,  "  t1/8:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 10, " •1/16:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 11, "  1/16:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 12, " t1/16:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 13, " •1/32:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 14, "  1/32:", NULL);
    nvgText(vg, pos.x, pos.y+v_s * 15, " t1/32:", NULL);

    std::stringstream to_display;
    to_display << std::fixed;
    to_display.precision(0);
    to_display.fill (' ');
    //bar
    to_display.width(4);
    to_display <<  module->bar  << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 0,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //1/2
    to_display.width(4);
    to_display << module->half_note_d << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 1,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->half_note << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 2,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->half_note_t << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 3,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //1/4
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->qt_note_d << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 4,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->qt_note << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 5,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->qt_note_t << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 6,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //1/8
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->eight_note_d << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 7,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->eight_note << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 8,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->eight_note_t << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 9,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //1/16
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->sixth_note_d << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 10,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->sixth_note << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 11,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->sixth_note_t << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 12,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //1/32
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->trth_note_d << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 13,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->trth_note << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 14,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(4);
    to_display << module->trth_note_t << " ms.";
    nvgText(vg, pos.x + h_off, pos.y + v_s * 15,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");

    to_display.precision(2);
    //hz bar
    to_display.width(5);
    to_display <<  module->hz_bar  << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 0,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //hz 1/2
    to_display.width(5);
    to_display << module->half_hz_d << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 1,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->half_hz << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 2,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->half_hz_t << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 3,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //hz 1/4
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->qt_hz_d << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 4,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->qt_hz << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 5,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->qt_hz_t << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 6,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //hz 1/8
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->eight_hz_d << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 7,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->eight_hz << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 8,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->eight_hz_t << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 9,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //hz 1/16
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->sixth_hz_d << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 10,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->sixth_hz << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 11,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->sixth_hz_t << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 12,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    //hz 1/32
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->trth_hz_d << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 13,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->trth_hz << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 14,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
    to_display.width(5);
    to_display << module->trth_hz_t << " hz.";
    nvgText(vg, pos.x + hz_txt_off, pos.y + v_s * 15,  to_display.str().c_str(), NULL);
    to_display.clear(); to_display.str("");
  
  }


  void draw(NVGcontext *vg) override{
    if (++frame >= 16){
      frame = 0;
    }
    drawMessage(vg, Vec(0, 0));
  }
};

//////////////////////////////////

struct BPMCalcWidget : ModuleWidget { 
    BPMCalcWidget(BPMCalc *module);
};

BPMCalcWidget::BPMCalcWidget(BPMCalc *module) : ModuleWidget(module) {

  setPanel(SVG::load(assetPlugin(plugin, "res/BPMCalc.svg")));

  //SCREWS
	addChild(Widget::create<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(Widget::create<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
  //BPM DISPLAY 
  BpmDisplayWidget *display = new BpmDisplayWidget();
  display->box.pos = Vec(43,54);
  display->box.size = Vec(45, 20);
  display->value = &module->bpm;
  addChild(display); 
    //TEMPO KNOB
  addParam(ParamWidget::create<as_KnobBlack>(Vec(130, 45), module, BPMCalc::TEMPO_PARAM, 30.0f, 300.0f, 120.0f));

   //LABEL DISPLAY 
  TxtDisplay *display1 = new TxtDisplay();
  display1->module = module;
  display1->box.pos = Vec(7, 120);
  display1->box.size = Vec(190, 240);
  addChild(display1);

}

Model *modelBPMCalc = Model::create<BPMCalc, BPMCalcWidget>("AS", "BPMCalc", "BPM to Delay/Hz Calculator", UTILITY_TAG);