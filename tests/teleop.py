# based on code from http://stackoverflow.com/questions/1835623/drawing-a-circle-using-wxpython ; taken without permission

import wx
from numpy import *
import serial

class SketchFrame(wx.Frame):
    def __init__(self, parent):

        wx.Frame.__init__(self, parent, -1, "Eric the Robot -- Tele-Op Console", size=(500,500))

        print "Opening serial port..."
        #self.serial_port = serial.Serial("/dev/cu.usbmodem1421", 9600)
        self.serial_port = serial.Serial("/dev/tty.usbserial-A700eFef", 9600)
        #self.serial_port = None

        self.sketch = SketchWindow(self, -1)
        self.Bind(wx.EVT_CLOSE, self.OnClose)

    def OnClose(self, event):
        print "Stopping..."
        map(self.serial_port.write,['V',chr(128), chr(128), '.'])
        print "Closing serial port..."
        self.serial_port.close()
        event.Skip()
        
class SketchWindow(wx.Window):
    def __init__ (self, parent,ID):
        wx.Window.__init__(self, parent, ID)

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBack)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnMouse)
        self.Bind(wx.EVT_MOTION, self.OnMouse)

        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnTimer)
        self.timer.Start(100)

        self.targetPos = wx.Point(0,0)
        self.serial_port = parent.serial_port

        self.circle_rad = 150

    def OnTimer(self, event):
        print "Sending serial command..."
        v_pos = mat(self.targetPos).T
    
        #v = min(self.circle_rad, linalg.norm(v_pos))
        v = -sign(float(v_pos[1])) * min(self.circle_rad, abs(float(v_pos[1])))
        w = sign(float(v_pos[0])) * min(self.circle_rad, abs(float(v_pos[0])))
        if v != 0: w = -w*sign(v)

        # normalize

        max_wheel_speed = 128
        max_v = max_wheel_speed #/2
        max_w = max_wheel_speed

        v = max_v*v/self.circle_rad
        w = max_w*w/self.circle_rad
        print v,w
        v_l = 128 + v - w/2
        v_r = 128 + v + w/2
        print v_l, v_r
        v_l = min(255, v_l)
        v_r = min(255, v_r)
        v_r = max(0, v_r)
        v_l = max(0, v_l)
        map(self.serial_port.write,['V',chr(int(v_l)), chr(int(v_r)), '.'])
        self.serial_port.flushInput()


    def OnMouse(self, event):
        if event.LeftIsDown():
            size = self.GetClientSize()
            center = wx.Point(size.width/2, size.height/2)
            self.targetPos = event.GetPosition() - center

        self.Refresh()
        self.Update()

    def OnEraseBack(self, event):
        pass # do nothing to avoid flicker

    def OnPaint(self, event):
        size=self.GetClientSize()
        center = wx.Point(size.width/2, size.height/2)

        dc = wx.PaintDC(self)
        dc.BeginDrawing()
        dc.Clear()
        pen = wx.Pen('blue',4)
        dc.SetPen(pen)
        dc.DrawCircle(center.x, center.y, self.circle_rad)
        dc.DrawLinePoint(center, center+self.targetPos)
        
        v_pos = mat(self.targetPos).T

        if linalg.norm(v_pos) != 0:
            v_orth = mat([[0, -1], [1, 0]]) * v_pos
            e_orth = v_orth/linalg.norm(v_orth)
            arrow_pts = [10*e_orth, 20*v_pos/linalg.norm(v_pos), -10*e_orth]
            arrow_pts = [wx.Point(float(v[0]),float(v[1])) for v in arrow_pts]  # convert to wx
        else:
            arrow_pts = []
        
        dc.DrawPolygon(arrow_pts, xoffset=center.x+self.targetPos.x, yoffset=center.y+self.targetPos.y)
        dc.EndDrawing()

if __name__=='__main__':
    app=wx.PySimpleApp()
    frame=SketchFrame(None)
    frame.Show(True)
    app.MainLoop()
