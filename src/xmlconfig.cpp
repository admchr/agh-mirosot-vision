
#include "amv.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <map>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cassert>

using namespace std;
using namespace xercesc;

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

char* itoa(int val, char* buf, int base) {
    assert(base==10);
    sprintf(buf, "%d", val);
    return buf;
}

int _access(const char*, int) {
    return 0;
}
void load_amv_config(const char* xmlFile, struct amv_config* config){
    map<const char*, pair<int, int>, ltstr> colorMap;
    try {
            XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
            //cout << "Error during initialization! :\n"
                 //<< message << "\n";
            XMLString::release(&message);
            return;
        }

        XercesDOMParser* parser = new XercesDOMParser();
        parser->setValidationScheme(XercesDOMParser::Val_Never);
        //parser->setIncludeIgnorableWhitespace(false);    // would be nice to get it working

        ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
        parser->setErrorHandler(errHandler);
        //cout<<xmlFile<<"\n";

        if(_access(xmlFile, 0) == -1)
            return;
            //cout << "file doesn't exist\n";


        try {
            parser->parse(xmlFile);
        }
        catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
            //cout << "Exception message is: \n"
            //     << message << "\n";
            XMLString::release(&message);
            return ;
        }
        catch (const DOMException& toCatch) {
            char* message = XMLString::transcode(toCatch.msg);
            //cout << "Exception message is: \n"
            //     << message << "\n";
            XMLString::release(&message);
            return ;
        }
        catch (...) {
            //cout << "Unexpected Exception \n" ;
            return ;
        }
        xercesc::DOMDocument *doc = parser->getDocument();
        DOMNode* root = doc->getDocumentElement();

        XMLCh tempStr[100];
        //memory leaks!!! need to frre pointer returned by transcode!

        XMLString::transcode("px_per_cm", tempStr, 99);
        config->px_per_cm = atof(XMLString::transcode(doc->getElementsByTagName(tempStr)->item(0)->getFirstChild()->getNodeValue()));

        XMLString::transcode("meanshift_radius", tempStr, 99);
        config->meanshift_radius = atoi(XMLString::transcode(doc->getElementsByTagName(tempStr)->item(0)->getFirstChild()->getNodeValue()));

        XMLString::transcode("meanshift_threshold", tempStr, 99);
        config->meanshift_threshold = atoi(XMLString::transcode(doc->getElementsByTagName(tempStr)->item(0)->getFirstChild()->getNodeValue()));

        XMLString::transcode("same_color_distance", tempStr, 99);
        config->same_color_distance = atoi(XMLString::transcode(doc->getElementsByTagName(tempStr)->item(0)->getFirstChild()->getNodeValue()));

        XMLString::transcode("minimum_saturation", tempStr, 99);
        config->minimum_saturation = atoi(XMLString::transcode(doc->getElementsByTagName(tempStr)->item(0)->getFirstChild()->getNodeValue()));

        XMLString::transcode("white_cutoff", tempStr, 99);
        config->white_cutoff = atoi(XMLString::transcode(doc->getElementsByTagName(tempStr)->item(0)->getFirstChild()->getNodeValue()));

        XMLString::transcode("black_cutoff", tempStr, 99);
        config->black_cutoff = atoi(XMLString::transcode(doc->getElementsByTagName(tempStr)->item(0)->getFirstChild()->getNodeValue()));

        XMLString::transcode("scale", tempStr, 99);
        DOMNodeList* scalePoint =  doc->getElementsByTagName(tempStr)->item(0)->getChildNodes();
        for(int i=0; i < scalePoint->getLength(); ++i){
            if(XMLString::equals(scalePoint->item(i)->getNodeName(), XMLString::transcode("point"))){
                DOMNodeList* pointCoordinates = scalePoint->item(i)->getChildNodes();
                for(int j=0; j<pointCoordinates->getLength(); ++j){
                    if(XMLString::equals(pointCoordinates->item(j)->getNodeName(), XMLString::transcode("x"))){
                        config->transform.output_scale.x = atof(XMLString::transcode(pointCoordinates->item(j)->getFirstChild()->getNodeValue()));
                    }
                    if(XMLString::equals(pointCoordinates->item(j)->getNodeName(), XMLString::transcode("y"))){
                        config->transform.output_scale.y = atof(XMLString::transcode(pointCoordinates->item(j)->getFirstChild()->getNodeValue()));
                    }
                }
            }
        }

        XMLString::transcode("play_field_boundary", tempStr, 99);
        DOMNodeList* borderPoint = doc->getElementsByTagName(tempStr)->item(0)->getChildNodes();
        pair<int, int> borderPoints[2];
        int borderPointIndex=0;
        for(int i=0; i < borderPoint->getLength(); ++i){
            if(XMLString::equals(borderPoint->item(i)->getNodeName(), XMLString::transcode("point"))){
                DOMNodeList* pointCoordinates = borderPoint->item(i)->getChildNodes();
                for(int j=0; j<pointCoordinates->getLength(); ++j){
                    if(XMLString::equals(pointCoordinates->item(j)->getNodeName(), XMLString::transcode("x"))){
                        borderPoints[borderPointIndex].first = atoi(XMLString::transcode(pointCoordinates->item(j)->getFirstChild()->getNodeValue()));
                    }
                    if(XMLString::equals(pointCoordinates->item(j)->getNodeName(), XMLString::transcode("y"))){
                        borderPoints[borderPointIndex].second = atoi(XMLString::transcode(pointCoordinates->item(j)->getFirstChild()->getNodeValue()));
                    }
                }
                ++borderPointIndex;
            }
        }
        if(borderPoints[0].first> borderPoints[1].first){
            swap(borderPoints[0],borderPoints[1]);
        }
        config->transform.field_top_left.x = borderPoints[0].first;
        config->transform.field_top_left.y = borderPoints[0].second;
        config->transform.field_bottom_right.x = borderPoints[1].first;
        config->transform.field_bottom_right.y = borderPoints[1].second;

        XMLString::transcode("colors", tempStr, 99);
        DOMNodeList* colors = doc->getElementsByTagName(tempStr)->item(0)->getChildNodes();
        //cout<<"len: "<<colors->getLength()<<"\n";
        for(int i=0; i < colors->getLength(); ++i){
            if(XMLString::equals(colors->item(i)->getNodeName(), XMLString::transcode("color"))){
                DOMNode* color = colors->item(i);
                DOMNodeList* colorParam = color->getChildNodes();
                char* name = NULL;
                double minHue,maxHue;
                //cout<<"colorParam len: "<< colorParam->getLength() <<"\n";
                for(int j=0; j<colorParam->getLength();++j){
                    if(XMLString::equals(colorParam->item(j)->getNodeName(), XMLString::transcode("name"))){
                        name = XMLString::transcode(colorParam->item(j)->getFirstChild()->getNodeValue());
                    }
                    if(XMLString::equals(colorParam->item(j)->getNodeName(), XMLString::transcode("min"))){
                        minHue = atof(XMLString::transcode(colorParam->item(j)->getFirstChild()->getNodeValue()));
                    }
                    if(XMLString::equals(colorParam->item(j)->getNodeName(), XMLString::transcode("max"))){
                        maxHue = atof(XMLString::transcode(colorParam->item(j)->getFirstChild()->getNodeValue()));
                    }
                }
                colorMap[name] = make_pair(minHue,maxHue);
                //cout<<"name: "<<XMLString::transcode(params->item(i)->getNodeName())<<" value: "<<XMLString::transcode(params->item(i)->getFirstChild()->getNodeValue())<<"\n";
            }

        }
        config->blue.color.hue_min = colorMap["blue"].first;
        config->blue.color.hue_max = colorMap["blue"].second;
        config->yellow.color.hue_min = colorMap["yellow"].first;
        config->yellow.color.hue_max = colorMap["yellow"].second;
        //config->white_is_yellow = 1;
        config->orange.hue_min = colorMap["orange"].first;
        config->orange.hue_max = colorMap["orange"].second;

        XMLString::transcode("team", tempStr, 99);
        DOMNodeList* teams = doc->getElementsByTagName(tempStr);//->item(0)->getChildNodes();
        for(int i=0; i<teams->getLength();++i){
            DOMNodeList* teamParams = teams->item(i)->getChildNodes();
            char* teamColor = NULL;
            int robots=0;
            bool home_team=false;
            for(int j=0; j<teamParams->getLength(); ++j){
                if(XMLString::equals(teamParams->item(j)->getNodeName(), XMLString::transcode("team_color"))){
                        teamColor = XMLString::transcode(teamParams->item(j)->getFirstChild()->getNodeValue());
                    }
                if(XMLString::equals(teamParams->item(j)->getNodeName(), XMLString::transcode("robot"))){
                        robots++;
                    }
                if(XMLString::equals(teamParams->item(j)->getNodeName(), XMLString::transcode("home_team"))){
                    home_team = (bool) strcmp("false", XMLString::transcode(teamParams->item(j)->getFirstChild()->getNodeValue()));
                    //home_team will have same value as it is in conf file
                }
            }
            struct amv_team_info* team;
            if(strcmp("yellow",teamColor)==0){
                //cout<<"yellow\n";
                team = &(config->yellow);
            }else{
                team = &(config->blue);
            }
            team->team_size = robots;
            //cout<<teamColor<<" "<<team->team_size<<"\n";
            team->home_team = home_team;
            map<const char*, int, ltstr> secondaryColorsMap;
            secondaryColorsMap["green"]=1;
            secondaryColorsMap["violet"]=0;
            secondaryColorsMap["red"]=2;

            team->secondary_colors[0].hue_min=colorMap["violet"].first;
            team->secondary_colors[0].hue_max=colorMap["violet"].second;
            team->secondary_colors[1].hue_min=colorMap["green"].first;
            team->secondary_colors[1].hue_max=colorMap["green"].second;
            team->secondary_colors[2].hue_min=colorMap["red"].first;
            team->secondary_colors[2].hue_max=colorMap["red"].second;

            if(home_team){
                int robotNumber=0;
                for(int j=0; j<teamParams->getLength(); ++j){
                    if(XMLString::equals(teamParams->item(j)->getNodeName(), XMLString::transcode("robot"))){
                        DOMNodeList* secondaryColorsFind = teamParams->item(j)->getChildNodes();
                        DOMNodeList* secondaryColorsList;
                        for(int k=0; k<secondaryColorsFind->getLength(); ++k){
                            if(XMLString::equals(secondaryColorsFind->item(k)->getNodeName(), XMLString::transcode("secondary_colors"))){
                                secondaryColorsList=secondaryColorsFind->item(k)->getChildNodes();
                            }
                        }
                        for(int k=0; k<secondaryColorsList->getLength(); ++k){
                            if(XMLString::equals(secondaryColorsList->item(k)->getNodeName(), XMLString::transcode("first"))){
                                char* name = XMLString::transcode(secondaryColorsList->item(k)->getFirstChild()->getNodeValue());
                                int colorIndex = secondaryColorsMap[name];
                                team->robot_info[robotNumber].front_color = colorIndex;
                            }
                            if(XMLString::equals(secondaryColorsList->item(k)->getNodeName(), XMLString::transcode("second"))){
                                char* name = XMLString::transcode(secondaryColorsList->item(k)->getFirstChild()->getNodeValue());
                                int colorIndex = secondaryColorsMap[name];
                                team->robot_info[robotNumber].back_color = colorIndex;
                            }
                        }
                    robotNumber++;
                    }
                }
            }
        }
    XMLString::transcode("white_points", tempStr, 99);
    DOMNodeList* whitePoints = doc->getElementsByTagName(tempStr)->item(0)->getChildNodes();
    vector<pair<int, int> > whitePointsVector;
    for(int i=0; i< whitePoints->getLength(); ++i){
        if(XMLString::equals(whitePoints->item(i)->getNodeName(), XMLString::transcode("point"))){
            pair<int, int> p;
            DOMNodeList* coordinates = whitePoints->item(i)->getChildNodes();
            for(int j=0; j<coordinates->getLength();++j){
                if(XMLString::equals(coordinates->item(j)->getNodeName(), XMLString::transcode("x"))){
                    p.first = atoi(XMLString::transcode(coordinates->item(j)->getFirstChild()->getNodeValue()));
                }
                if(XMLString::equals(coordinates->item(j)->getNodeName(), XMLString::transcode("y"))){
                    p.second = atoi(XMLString::transcode(coordinates->item(j)->getFirstChild()->getNodeValue()));
                }
            }
            whitePointsVector.push_back(p);
        }
    }
    config->white_points_len =  whitePointsVector.size();
    config->white_points = new amv_image_pos[whitePointsVector.size()];
    for(int i=0; i< whitePointsVector.size(); ++i){
        config->white_points[i].x = whitePointsVector[i].first;
        config->white_points[i].y = whitePointsVector[i].second;
        //cout<<"x y "<<config->white_points[i].x<<" "<<config->white_points[i].x<<"\n";
    }

    XMLString::transcode("mask_points", tempStr, 99);
    DOMNodeList* maskPoints = doc->getElementsByTagName(tempStr)->item(0)->getChildNodes();
    vector<pair<int, int> > maskPointsVector;
    for(int i=0; i< maskPoints->getLength(); ++i){
        if(XMLString::equals(maskPoints->item(i)->getNodeName(), XMLString::transcode("point"))){
            pair<int, int> p;
            DOMNodeList* coordinates = maskPoints->item(i)->getChildNodes();
            for(int j=0; j<coordinates->getLength();++j){
                if(XMLString::equals(coordinates->item(j)->getNodeName(), XMLString::transcode("x"))){
                    p.first = atoi(XMLString::transcode(coordinates->item(j)->getFirstChild()->getNodeValue()));
                }
                if(XMLString::equals(coordinates->item(j)->getNodeName(), XMLString::transcode("y"))){
                    p.second = atoi(XMLString::transcode(coordinates->item(j)->getFirstChild()->getNodeValue()));
                }
            }
            maskPointsVector.push_back(p);
        }
    }
    config->mask_points_len =  maskPointsVector.size();
    config->mask_points = new amv_image_pos[maskPointsVector.size()];
    for(int i=0; i< maskPointsVector.size(); ++i){
        config->mask_points[i].x = maskPointsVector[i].first;
        config->mask_points[i].y = maskPointsVector[i].second;
        //cout<<"x y "<<config->mask_points[i].x<<" "<<config->mask_points[i].x<<"\n";
    }

    config->linearize = 0;

        //cout<<XMLString::transcode(root->getNodeName())<<"\n";
        //DOMNodeList* children = root->getChildNodes();
        /*cout<<"len: "<<params->getLength()<<"\n";
        for(int i=0; i < params->getLength(); ++i){
            if(!XMLString::equals(params->item(i)->getNodeName(), XMLString::transcode("#text")))
                cout<<"name: "<<XMLString::transcode(params->item(i)->getNodeName())<<" value: "<<XMLString::transcode(params->item(i)->getFirstChild()->getNodeValue())<<"\n";
        }*/
        delete parser;
        delete errHandler;

}

int save_amv_config(const char* xmlFile, struct amv_config* config){
    try {
            XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
            //cout << "Error during initialization! :\n"
            //     << message << "\n";
            XMLString::release(&message);
            return 1;
        }
        XMLCh tempStr[100];
        XMLString::transcode("Range", tempStr, 99);
        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

        XMLString::transcode("amv_config", tempStr, 99);
        xercesc::DOMDocument*   doc = impl->createDocument(0, tempStr, 0);
        DOMElement*   root = doc->getDocumentElement();


        XMLString::transcode("algorithm_parameters", tempStr, 99);
        DOMElement*   algorithm_parameters = doc->createElement(tempStr);
        root->appendChild(algorithm_parameters);

        XMLString::transcode("white_points", tempStr, 99);
        DOMElement*   whitePoints = doc->createElement(tempStr);

        for(int i=0; i<config->white_points_len; ++i){
            char coordinateValue[11];
            XMLString::transcode("point", tempStr, 99);
            DOMElement* point = doc->createElement(tempStr);
            XMLString::transcode("x", tempStr, 99);
            DOMElement* x = doc->createElement(tempStr);
            XMLString::transcode("y", tempStr, 99);
            DOMElement* y = doc->createElement(tempStr);

            itoa(config->white_points[i].x, coordinateValue, 10);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* xValue = doc->createTextNode(tempStr);

            itoa(config->white_points[i].y, coordinateValue, 10);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* yValue = doc->createTextNode(tempStr);

            x->appendChild(xValue);
            y->appendChild(yValue);
            point->appendChild(x);
            point->appendChild(y);
            whitePoints->appendChild(point);
        }

        XMLString::transcode("mask_points", tempStr, 99);
        DOMElement*   maskPoints = doc->createElement(tempStr);

        for(int i=0; i<config->mask_points_len; ++i){
            char coordinateValue[11];
            XMLString::transcode("point", tempStr, 99);
            DOMElement* point = doc->createElement(tempStr);
            XMLString::transcode("x", tempStr, 99);
            DOMElement* x = doc->createElement(tempStr);
            XMLString::transcode("y", tempStr, 99);
            DOMElement* y = doc->createElement(tempStr);

            itoa(config->mask_points[i].x, coordinateValue, 10);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* xValue = doc->createTextNode(tempStr);

            itoa(config->mask_points[i].y, coordinateValue, 10);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* yValue = doc->createTextNode(tempStr);

            x->appendChild(xValue);
            y->appendChild(yValue);
            point->appendChild(x);
            point->appendChild(y);
            maskPoints->appendChild(point);
        }

        algorithm_parameters->appendChild(whitePoints);
        algorithm_parameters->appendChild(maskPoints);

        char value[30];

        XMLString::transcode("px_per_cm", tempStr, 99);
        DOMElement*   pxPerCm = doc->createElement(tempStr);
        sprintf(value, "%f", config->px_per_cm);
        XMLString::transcode(value, tempStr, 99);
        DOMText* pxPerCmValue = doc->createTextNode(tempStr);
        pxPerCm->appendChild(pxPerCmValue);
        algorithm_parameters->appendChild(pxPerCm);

        XMLString::transcode("meanshift_radius", tempStr, 99);
        DOMElement*   meanshiftRadius = doc->createElement(tempStr);
        sprintf(value, "%i", config->meanshift_radius);
        XMLString::transcode(value, tempStr, 99);
        DOMText* meanshiftRadiusValue = doc->createTextNode(tempStr);
        meanshiftRadius->appendChild(meanshiftRadiusValue);
        algorithm_parameters->appendChild(meanshiftRadius);

        XMLString::transcode("meanshift_threshold", tempStr, 99);
        DOMElement*   meanshiftThreshold = doc->createElement(tempStr);
        sprintf(value, "%i", config->meanshift_threshold);
        XMLString::transcode(value, tempStr, 99);
        DOMText* meanshiftThresholdValue = doc->createTextNode(tempStr);
        meanshiftThreshold->appendChild(meanshiftThresholdValue);
        algorithm_parameters->appendChild(meanshiftThreshold);

        XMLString::transcode("same_color_distance", tempStr, 99);
        DOMElement*   sameColorDistance = doc->createElement(tempStr);
        sprintf(value, "%i", config->same_color_distance);
        XMLString::transcode(value, tempStr, 99);
        DOMText* sameColorDistanceValue = doc->createTextNode(tempStr);
        sameColorDistance->appendChild(sameColorDistanceValue);
        algorithm_parameters->appendChild(sameColorDistance);

        XMLString::transcode("black_cutoff", tempStr, 99);
        DOMElement*   blackCutoff = doc->createElement(tempStr);
        sprintf(value, "%i", config->black_cutoff);
        XMLString::transcode(value, tempStr, 99);
        DOMText* blackCutoffValue = doc->createTextNode(tempStr);
        blackCutoff->appendChild(blackCutoffValue);
        algorithm_parameters->appendChild(blackCutoff);

        XMLString::transcode("white_cutoff", tempStr, 99);
        DOMElement*   whiteCutoff = doc->createElement(tempStr);
        sprintf(value, "%i", config->white_cutoff);
        XMLString::transcode(value, tempStr, 99);
        DOMText* whiteCutoffValue = doc->createTextNode(tempStr);
        whiteCutoff->appendChild(whiteCutoffValue);
        algorithm_parameters->appendChild(whiteCutoff);

        XMLString::transcode("minimum_saturation", tempStr, 99);
        DOMElement*   minimumSaturation = doc->createElement(tempStr);
        sprintf(value, "%i", config->minimum_saturation);
        XMLString::transcode(value, tempStr, 99);
        DOMText* minimumSaturationValue = doc->createTextNode(tempStr);
        minimumSaturation->appendChild(minimumSaturationValue);
        algorithm_parameters->appendChild(minimumSaturation);


        XMLString::transcode("coordinate_transformation", tempStr, 99);
        DOMElement*   coordinateTransformation = doc->createElement(tempStr);
        XMLString::transcode("scale", tempStr, 99);
        DOMElement*   scale = doc->createElement(tempStr);
        XMLString::transcode("play_field_boundary", tempStr, 99);
        DOMElement*   playFieldBoundary = doc->createElement(tempStr);
        coordinateTransformation->appendChild(scale);
        coordinateTransformation->appendChild(playFieldBoundary);
        algorithm_parameters->appendChild(coordinateTransformation);
        {
            XMLString::transcode("point", tempStr, 99);
            DOMElement*   point = doc->createElement(tempStr);
            char coordinateValue[20];
            XMLString::transcode("x", tempStr, 99);
            DOMElement* x = doc->createElement(tempStr);
            XMLString::transcode("y", tempStr, 99);
            DOMElement* y = doc->createElement(tempStr);

            sprintf(coordinateValue, "%f", config->transform.output_scale.x);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* xValue = doc->createTextNode(tempStr);

            sprintf(coordinateValue, "%f", config->transform.output_scale.y);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* yValue = doc->createTextNode(tempStr);
            point->appendChild(x);
            point->appendChild(y);
            x->appendChild(xValue);
            y->appendChild(yValue);
            scale->appendChild(point);
        }
        {
            XMLString::transcode("point", tempStr, 99);
            DOMElement*   point = doc->createElement(tempStr);
            char coordinateValue[20];
            XMLString::transcode("x", tempStr, 99);
            DOMElement* x = doc->createElement(tempStr);
            XMLString::transcode("y", tempStr, 99);
            DOMElement* y = doc->createElement(tempStr);

            sprintf(coordinateValue, "%i", config->transform.field_top_left.x);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* xValue = doc->createTextNode(tempStr);

            sprintf(coordinateValue, "%i", config->transform.field_top_left.y);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* yValue = doc->createTextNode(tempStr);
            point->appendChild(x);
            point->appendChild(y);
            x->appendChild(xValue);
            y->appendChild(yValue);
            playFieldBoundary->appendChild(point);
        }
        {
            XMLString::transcode("point", tempStr, 99);
            DOMElement*   point = doc->createElement(tempStr);
            char coordinateValue[20];
            XMLString::transcode("x", tempStr, 99);
            DOMElement* x = doc->createElement(tempStr);
            XMLString::transcode("y", tempStr, 99);
            DOMElement* y = doc->createElement(tempStr);

            sprintf(coordinateValue, "%i", config->transform.field_bottom_right.x);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* xValue = doc->createTextNode(tempStr);

            sprintf(coordinateValue, "%i", config->transform.field_bottom_right.y);
            XMLString::transcode(coordinateValue, tempStr, 99);
            DOMText* yValue = doc->createTextNode(tempStr);
            point->appendChild(x);
            point->appendChild(y);
            x->appendChild(xValue);
            y->appendChild(yValue);
            playFieldBoundary->appendChild(point);
        }


        XMLString::transcode("colors", tempStr, 99);
        DOMElement*   colors = doc->createElement(tempStr);

        map<const char*, pair<int, int>, ltstr> colorMap;
        colorMap["blue"] = make_pair(config->blue.color.hue_min, config->blue.color.hue_max);
        colorMap["yellow"] = make_pair(config->yellow.color.hue_min, config->yellow.color.hue_max);
        colorMap["orange"] = make_pair(config->orange.hue_min, config->orange.hue_max);
        colorMap["green"] = make_pair(config->yellow.secondary_colors[1].hue_min, config->yellow.secondary_colors[1].hue_max);
        colorMap["violet"] = make_pair(config->yellow.secondary_colors[0].hue_min, config->yellow.secondary_colors[0].hue_max);
        colorMap["red"] = make_pair(config->yellow.secondary_colors[2].hue_min, config->yellow.secondary_colors[2].hue_max);
        char* colorsIndexes[] = {"violet", "green", "red"};

        map<const char*, pair<int, int>, ltstr>::iterator it;
        for(it=colorMap.begin(); it!= colorMap.end(); ++it){
            XMLString::transcode("name", tempStr, 99);
            DOMElement*   name = doc->createElement(tempStr);
            XMLString::transcode(it->first, tempStr, 99);
            DOMText* nameValue = doc->createTextNode(tempStr);
            name->appendChild(nameValue);

            XMLString::transcode("min", tempStr, 99);
            DOMElement*   hueMin = doc->createElement(tempStr);
            sprintf(value, "%i", it->second.first);
            XMLString::transcode(value, tempStr, 99);
            DOMText* hueMinValue = doc->createTextNode(tempStr);
            hueMin->appendChild(hueMinValue);

            XMLString::transcode("max", tempStr, 99);
            DOMElement*   hueMax = doc->createElement(tempStr);
            sprintf(value, "%i", it->second.second);
            XMLString::transcode(value, tempStr, 99);
            DOMText* hueMaxValue = doc->createTextNode(tempStr);
            hueMax->appendChild(hueMaxValue);

            XMLString::transcode("color", tempStr, 99);
            DOMElement*   color = doc->createElement(tempStr);

            color->appendChild(name);
            color->appendChild(hueMin);
            color->appendChild(hueMax);
            colors->appendChild(color);
        }
        algorithm_parameters->appendChild(colors);

        XMLString::transcode("teams", tempStr, 99);
        DOMElement*   teams = doc->createElement(tempStr);

        {//blue team
            XMLString::transcode("team", tempStr, 99);
            DOMElement*   team = doc->createElement(tempStr);

            XMLString::transcode("team_color", tempStr, 99);
            DOMElement*   teamColor = doc->createElement(tempStr);
            XMLString::transcode("blue", tempStr, 99);
            DOMText* teamColorValue = doc->createTextNode(tempStr);
            teamColor->appendChild(teamColorValue);

            XMLString::transcode("home_team", tempStr, 99);
            DOMElement*   homeTeam = doc->createElement(tempStr);
            if(config->blue.home_team){
                XMLString::transcode("true", tempStr, 99);
            }else{
                XMLString::transcode("false", tempStr, 99);
            }
            DOMText* homeTeamValue = doc->createTextNode(tempStr);
            homeTeam->appendChild(homeTeamValue);

            for(int i=0; i<config->blue.team_size; ++i){
                XMLString::transcode("robot", tempStr, 99);
                DOMElement* robot = doc->createElement(tempStr);
                if(config->blue.home_team){
                    XMLString::transcode("secondary_colors", tempStr, 99);
                    DOMElement* secondaryColors = doc->createElement(tempStr);
                    XMLString::transcode("first", tempStr, 99);
                    DOMElement* first = doc->createElement(tempStr);
                    XMLString::transcode("second", tempStr, 99);
                    DOMElement* second = doc->createElement(tempStr);

                    XMLString::transcode(colorsIndexes[config->blue.robot_info[i].front_color], tempStr, 99);
                    DOMText* firstValue = doc->createTextNode(tempStr);
                    first->appendChild(firstValue);

                    XMLString::transcode(colorsIndexes[config->blue.robot_info[i].back_color], tempStr, 99);
                    DOMText* secondValue = doc->createTextNode(tempStr);
                    second->appendChild(secondValue);

                    secondaryColors->appendChild(first);
                    secondaryColors->appendChild(second);
                    robot->appendChild(secondaryColors);
                }
                team->appendChild(robot);
            }
            team->appendChild(teamColor);
            team->appendChild(homeTeam);
            teams->appendChild(team);
        }

        {//yellow team
            XMLString::transcode("team", tempStr, 99);
            DOMElement*   team = doc->createElement(tempStr);

            XMLString::transcode("team_color", tempStr, 99);
            DOMElement*   teamColor = doc->createElement(tempStr);
            XMLString::transcode("yellow", tempStr, 99);
            DOMText* teamColorValue = doc->createTextNode(tempStr);
            teamColor->appendChild(teamColorValue);

            XMLString::transcode("home_team", tempStr, 99);
            DOMElement*   homeTeam = doc->createElement(tempStr);
            if(config->yellow.home_team){
                XMLString::transcode("true", tempStr, 99);
            }else{
                XMLString::transcode("false", tempStr, 99);
            }
            DOMText* homeTeamValue = doc->createTextNode(tempStr);
            homeTeam->appendChild(homeTeamValue);

            for(int i=0; i<config->yellow.team_size; ++i){
                XMLString::transcode("robot", tempStr, 99);
                DOMElement* robot = doc->createElement(tempStr);
                if(config->yellow.home_team){
                    XMLString::transcode("secondary_colors", tempStr, 99);
                    DOMElement* secondaryColors = doc->createElement(tempStr);
                    XMLString::transcode("first", tempStr, 99);
                    DOMElement* first = doc->createElement(tempStr);
                    XMLString::transcode("second", tempStr, 99);
                    DOMElement* second = doc->createElement(tempStr);

                    XMLString::transcode(colorsIndexes[config->yellow.robot_info[i].front_color], tempStr, 99);
                    DOMText* firstValue = doc->createTextNode(tempStr);
                    first->appendChild(firstValue);

                    XMLString::transcode(colorsIndexes[config->yellow.robot_info[i].back_color], tempStr, 99);
                    DOMText* secondValue = doc->createTextNode(tempStr);
                    second->appendChild(secondValue);

                    secondaryColors->appendChild(first);
                    secondaryColors->appendChild(second);
                    robot->appendChild(secondaryColors);
                }
                team->appendChild(robot);
            }
            team->appendChild(teamColor);
            team->appendChild(homeTeam);
            teams->appendChild(team);
        }

        root->appendChild(teams);
        XMLString::transcode("LS", tempStr, 99);
        impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
        DOMLSSerializer* theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();

        // optionally you can set some features on this serializer
        /*if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTDiscardDefaultContent, true))
            theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent, true);
            */
        if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
             theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

        // StdOutFormatTarget prints the resultant XML stream
        // to stdout once it receives any thing from the serializer.
        XMLFormatTarget *myFormTarget = new LocalFileFormatTarget(xmlFile);
        DOMLSOutput* theOutput = ((DOMImplementationLS*)impl)->createLSOutput();
        theOutput->setByteStream(myFormTarget);

        try {
            // do the serialization through DOMLSSerializer::write();
            theSerializer->write(doc, theOutput);
        }
        catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
            //cout << "Exception message is: \n"
                 //<< message << "\n";
            XMLString::release(&message);
            return -1;
        }
        catch (const DOMException& toCatch) {
            char* message = XMLString::transcode(toCatch.msg);
            //cout << "Exception message is: \n"
                 //<< message << "\n";
            XMLString::release(&message);
            return -1;
        }
        catch (...) {
            //cout << "Unexpected Exception \n" ;
            return -1;
        }

        theOutput->release();
        theSerializer->release();
        doc->release();
        delete myFormTarget;
        return 0;
}


