//
//  DisconnectedViewController.swift
//  Cool Beans
//
//  Created by Kyle on 11/14/14.
//  Copyright (c) 2014 Kyle Weiner. All rights reserved.
//

import UIKit

class DisconnectedViewController: UIViewController, PTDBeanManagerDelegate, UITableViewDelegate, UITableViewDataSource {

    let connectedViewControllerSegueIdentifier = "ViewConnection"
    var beanArray : [PTDBean] = [PTDBean]()

    var manager: PTDBeanManager!
    var connectedBean: PTDBean? {
        didSet {
            if connectedBean == nil {
                beanManagerDidUpdateState(manager)
            } else {
                performSegueWithIdentifier(connectedViewControllerSegueIdentifier, sender: self)
            }
        }
    }

    @IBOutlet weak var activityIndicator: UIActivityIndicatorView!

    @IBOutlet weak var beanTableView: UITableView!
    // MARK: Lifecycle

    override func viewDidLoad() {
        super.viewDidLoad()
        manager = PTDBeanManager(delegate: self)
    }

    // MARK: Navigation

    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if segue.identifier == connectedViewControllerSegueIdentifier {
            let vc = segue.destinationViewController as! ConnectedViewController
            vc.connectedBean = connectedBean
            vc.connectedBean?.delegate = vc
        }
    }

    // MARK: PTDBeanManagerDelegate

    func beanManagerDidUpdateState(beanManager: PTDBeanManager!) {
        switch beanManager.state {
            case .Unsupported:
                let alertController = UIAlertController(title: "Error", message: "This device is unsupported", preferredStyle: .Alert)
                
                let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
                alertController.addAction(defaultAction)
                
                presentViewController(alertController, animated: true, completion: nil)
            case .PoweredOff:
                let alertController = UIAlertController(title: "Error", message: "Please turn on Bluetooth", preferredStyle: .Alert)
                
                let defaultAction = UIAlertAction(title: "OK", style: .Default, handler: nil)
                alertController.addAction(defaultAction)
                
                presentViewController(alertController, animated: true, completion: nil)
                
            case .PoweredOn:
                beanManager.startScanningForBeans_error(nil);
            default:
                break
        }
    }

    func beanManager(beanManager: PTDBeanManager!, didDiscoverBean bean: PTDBean!, error: NSError!) {
        print("DISCOVERED BEAN \nName: \(bean.name), UUID: \(bean.identifier) RSSI: \(bean.RSSI)")
        if connectedBean == nil {
            if bean.state == .Discovered {
                //manager.connectToBean(bean, error: nil)
                beanArray.append(bean)
                beanTableView.reloadData()
            }
        }
    }

    func BeanManager(beanManager: PTDBeanManager!, didConnectToBean bean: PTDBean!, error: NSError!) {
        print("CONNECTED BEAN \nName: \(bean.name), UUID: \(bean.identifier) RSSI: \(bean.RSSI)")
        if connectedBean == nil {
            connectedBean = bean
        }
    }

    func beanManager(beanManager: PTDBeanManager!, didDisconnectBean bean: PTDBean!, error: NSError!) {
        print("DISCONNECTED BEAN \nName: \(bean.name), UUID: \(bean.identifier) RSSI: \(bean.RSSI)")

        // Dismiss any modal view controllers.
        presentedViewController?.dismissViewControllerAnimated(true, completion: {
            self.dismissViewControllerAnimated(true, completion: nil)
        })

        self.connectedBean = nil
    }
    
    // MARK: table functions
    func numberOfRowsInTableView(aTableView: UITableView) -> Int
    {
        let numberOfRows:Int = beanArray.count
        return numberOfRows
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return beanArray.count
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier( "aCell", forIndexPath: indexPath)
        cell.textLabel!.text = self.beanArray[indexPath.row].name
        
        return cell
    }
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        print("selected row: " + String(indexPath.row))
        manager.connectToBean(self.beanArray[indexPath.row], error: nil)
        activityIndicator.startAnimating()
    }

 }
