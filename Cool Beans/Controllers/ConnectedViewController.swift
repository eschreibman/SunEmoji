//
//  ConnectedViewController.swift
//  Cool Beans
//
//  Created by Kyle on 11/14/14.
//  Copyright (c) 2014 Kyle Weiner. All rights reserved.
//

import UIKit

struct Temperature {
    enum State {
        case Unknown
        case Cold
        case Cool
        case Warm
        case Hot
    }

    var degreesCelcius: Float
    var degressFahrenheit: Float {
        return (degreesCelcius * 1.8) + 32.0
    }

    func state() -> State {
        let fahrenheit = degressFahrenheit
        switch Int(fahrenheit) {
            case _ where fahrenheit <= 39:
                return .Cold
            case 40...65:
                return .Cool
            case 66...80:
                return .Warm
            case _ where fahrenheit >= 81:
                return .Hot
            default:
                return .Unknown
        }
    }
}

class ConnectedViewController: UIViewController, PTDBeanDelegate {

    let refreshControl = UIRefreshControl()

    var connectedBean: PTDBean?
    var currentTemperature: Temperature = Temperature(degreesCelcius: 0.0) {
        didSet {
            updateTemperatureView()
            updateBean()
        }
    }

    @IBOutlet weak var scrollView: UIScrollView!
    @IBOutlet weak var temperatureView: TemperatureView!

    // MARK: Lifecycle

    override func viewDidLoad() {
        super.viewDidLoad()

        // Update the name label.
        temperatureView.nameLabel.text = connectedBean?.name

        // Add pull-to-refresh control.
        refreshControl.addTarget(self, action: "didPullToRefresh:", forControlEvents: .ValueChanged)
        refreshControl.tintColor = .whiteColor()
        scrollView.addSubview(refreshControl)
            temperatureView.xAcc.text = String(format:"%f", 0.0)
        temperatureView.yAcc.text = String(format:"%f", 0.0)
        temperatureView.zAcc.text = String(format:"%f", 0.0)
        // set up timer
        if ((temperatureView.timer == nil)){
            temperatureView.timer = NSTimer.scheduledTimerWithTimeInterval(0.1, target:self, selector: Selector("updateAcc"), userInfo: nil, repeats: true)
        }

    }

    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        connectedBean?.readTemperature()
    }

    // MARK: Actions

    func didPullToRefresh(sender: AnyObject) {
        refreshControl.endRefreshing()
        temperatureView.xAcc.text = String(format:"%f", 0.0)
        temperatureView.yAcc.text = String(format:"%f", 0.0)
        temperatureView.zAcc.text = String(format:"%f", 0.0)
    }
    
    func updateAcc(){
        connectedBean?.readTemperature()
        connectedBean?.readAccelerationAxes()
    }

    // MARK: PTDBeanDelegate

    func bean(bean: PTDBean!, didUpdateTemperature degrees_celsius: NSNumber!) {
        let newTemperature = Temperature(degreesCelcius: degrees_celsius.floatValue)
        
        if newTemperature.degreesCelcius != currentTemperature.degreesCelcius {
            print("TEMPERATURE UPDATED \nOld: \(currentTemperature.degressFahrenheit)℉ \nNew: \(newTemperature.degressFahrenheit)℉")
            currentTemperature = newTemperature
        }
    }

    func bean(bean: PTDBean!, didUpdateAccelerationAxes acceleration: PTDAcceleration) {
        temperatureView.xAcc.text = String(format:"%f", acceleration.x)
        temperatureView.yAcc.text = String(format:"%f", acceleration.y)
        temperatureView.zAcc.text = String(format:"%f", acceleration.z)
    }

    // MARK: Helper

    func updateTemperatureView() {
        // Update the temperature label.
        temperatureView.temperatureLabel.text = String(format: "%.f℉", currentTemperature.degressFahrenheit)

        // Update the background color.
        var backgroundColor: UIColor

        switch currentTemperature.state() {
            case .Unknown:
                backgroundColor = .blackColor()
            case .Cold:
                backgroundColor = .CBColdColor()
            case .Cool:
                backgroundColor = .CBCoolColor()
            case .Warm:
                backgroundColor = .CBWarmColor()
            case .Hot:
                backgroundColor = .CBHotColor()
        }

        UIView.animateWithDuration(0.4, animations: {
            self.scrollView.backgroundColor = backgroundColor
            self.temperatureView.containerView.backgroundColor = backgroundColor
        })
    }

    func updateBean() {
        connectedBean?.setLedColor(temperatureView.containerView.backgroundColor)
    }

}