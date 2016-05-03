//
//  ConnectedViewController.swift
//  Cool Beans
//
//  Created by Kyle on 11/14/14.
//  Copyright (c) 2014 Kyle Weiner. All rights reserved.
//

import UIKit
import Charts
import AudioToolbox

class ConnectedViewController: UIViewController, PTDBeanDelegate {

    var connectedBean: PTDBean?
    
    var time : [String] = []
    var uvIndices : [Double] = []

    //@IBOutlet weak var scrollView: UIScrollView!
    @IBOutlet weak var sonicView: SonicView!
    
   

    // MARK: Lifecycle

    override func viewDidLoad() {
        super.viewDidLoad()

        // Update the name label.
        sonicView.nameLabel.text = connectedBean?.name
        
        sonicView.myController = self;
        
        sonicView.resetBtnImage.layer.cornerRadius = 10.0;
        sonicView.resetBtnImage.backgroundColor = UIColor(colorLiteralRed: 0.0, green: 1.0, blue: 0.0, alpha: 1.0)
        
        sonicView.putOnSunscreenLabel.hidden = true;
        
    }

    
    func setChart(dataPoints: [String], values: [Double]) {
        
        var dataEntries: [ChartDataEntry] = []
        var value = 0.0;
        
        for i in 0..<dataPoints.count {
            
            if(values[i] > 12){
                value = 12
            }
            else {
                value = values[i]
            }
            
            let dataEntry = ChartDataEntry(value: value, xIndex: i)
            dataEntries.append(dataEntry)
        }
        
        
        let lineChartDataSet = LineChartDataSet(yVals: dataEntries, label: "UV Index")
        let lineChartData = LineChartData(xVals: dataPoints, dataSet: lineChartDataSet)
        
       
        sonicView.lineChartView.leftAxis.axisMinValue = 0.0
        sonicView.lineChartView.rightAxis.axisMinValue = 0.0
        sonicView.lineChartView.leftAxis.axisMaxValue =  12.0
        sonicView.lineChartView.rightAxis.axisMinValue = 12.0
        
        sonicView.lineChartView.xAxis.axisMinValue = 0;
        
        sonicView.lineChartView.descriptionText = ""
        //sonicView.lineChartView.legend.enabled = false
        sonicView.lineChartView.xAxis.labelPosition = .BottomInside
        
        

        
        sonicView.lineChartView.leftAxis.valueFormatter = NSNumberFormatter()
        sonicView.lineChartView.leftAxis.valueFormatter?.minimumFractionDigits = 0
        
        //sonicView.lineChartView.layer.cornerRadius = 8.0
        
        sonicView.lineChartView.data = lineChartData
        
    }

    
    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        connectedBean?.readTemperature()
        setChart(time, values: uvIndices)
    }

    // MARK: Actions

    func updateAcc(){
        connectedBean?.readTemperature()
        connectedBean?.readAccelerationAxes()
    }

    // MARK: PTDBeanDelegate
    
    func bean(bean: PTDBean!, serialDataReceived data:NSData) {
        let theString:String = NSString(data: data, encoding: NSASCIIStringEncoding)! as String
        
        //string coming in looks like 'index:20'

        if(theString.containsString("index:")){
            let stringParts = theString.componentsSeparatedByString(":")
            sonicView.uvIndex.text = stringParts[1]
            
            if(uvIndices.count >= 10){
                time.removeFirst()
                uvIndices.removeFirst()
            }
            time.append(getCurrentTime())
            uvIndices.append(Double(stringParts[1])!)
            
            setChart(time, values: uvIndices)
            
        }
        
        
        if(theString.containsString("timer_at_zero")){
            
            let notification = UILocalNotification()
            notification.alertBody = "Put on sunscreen!" // text that will be displayed in the notification
            notification.alertAction = "Open"
            notification.fireDate = NSDate() // date when notification should be fired (NSDate() defaults to current date and time)
            notification.soundName = UILocalNotificationDefaultSoundName // play default sound (don’t think this’ll work unless we add “.Sound” to the AppDelegate)
            notification.category = "TODO_CATEGORY" // i don’t think we’re using this
            UIApplication.sharedApplication().scheduleLocalNotification(notification) // schedules the notification
            
            sonicView.resetButton.setTitleColor(UIColor(colorLiteralRed: 1.0, green: 0.0, blue: 0.0, alpha: 1.0), forState:UIControlState.Normal)
            sonicView.resetBtnImage.backgroundColor = UIColor(colorLiteralRed: 1.0, green: 0.0, blue: 0.0, alpha: 1.0)

            AudioServicesPlayAlertSound(SystemSoundID(kSystemSoundID_Vibrate))
            AudioServicesPlayAlertSound(SystemSoundID(kSystemSoundID_Vibrate))
            
            
            sonicView.putOnSunscreenLabel.hidden = false;
            
        }
        
        
        if(theString.containsString("pbutton_pressed")){
            
            sonicView.resetButton.setTitleColor(UIColor(colorLiteralRed: 0.0, green: 1.0, blue: 0.0, alpha: 1.0), forState:UIControlState.Normal)
            sonicView.resetBtnImage.backgroundColor = UIColor(colorLiteralRed: 0.0, green: 1.0, blue: 0.0, alpha: 1.0)
            
            sonicView.putOnSunscreenLabel.hidden = true;
            
        }
        
        
        if(theString.containsString("reset_confirm")){
        
            sonicView.resetButton.setTitleColor(UIColor(colorLiteralRed: 0.0, green: 1.0, blue: 0.0, alpha: 1.0), forState:UIControlState.Normal)

            sonicView.resetBtnImage.backgroundColor = UIColor(colorLiteralRed: 0.0, green: 1.0, blue: 0.0, alpha: 1.0)
            
            sonicView.resetButton.enabled  = true;
            
            sonicView.putOnSunscreenLabel.hidden = true;

        
        }
        
        
        print(theString)

    }
    
    func getCurrentTime() -> String{
        let date = NSDate()
        let calendar = NSCalendar.currentCalendar()
        let hour = calendar.component(.Hour, fromDate: date)
        let minutes = calendar.component(.Minute, fromDate: date)
        
        var minuteString = String(minutes)
        var hourString = String(hour)
        
        if(minutes < 10){
            minuteString = "0" + String(minutes)
        }
        if(hour > 12){
            hourString = String(hour - 12)
        }
        
        
        let string = hourString + ":" + minuteString
        
        return string
        
        
    }
    
    override func preferredStatusBarStyle() -> UIStatusBarStyle {
        return .LightContent
    }


    // MARK: Helper

    func updatesonicView() {


    }

    func updateBean() {
        connectedBean?.setLedColor(sonicView.containerView.backgroundColor)
    }

}