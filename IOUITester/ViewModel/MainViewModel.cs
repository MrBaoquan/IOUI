using IOToolkit;
using ReactiveUI;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IOUITester.ViewModel
{
    public class MainViewModel : ReactiveObject, IActivatableViewModel
    {
        public ViewModelActivator Activator { get; }

        public MainViewModel()
        {
            Activator = new ViewModelActivator();
            
            this.WhenActivated(disposables =>
            {
                IODeviceController.Load();
                var _idx = 0;
                Observable.Interval(TimeSpan.FromSeconds(1))
                .ObserveOn(RxApp.MainThreadScheduler)
                .Subscribe(_ =>
                {
                    if(_idx++ % 2 == 0)
                    {
                        Debug.WriteLine("load");
                        IODeviceController.Load();
                    }
                    else
                    {
                        Debug.WriteLine("unload");
                        IODeviceController.Unload();
                    }
                });

                var ioDev1 = IODeviceController.GetIODevice($"extDev1");
                ioDev1.BindAction("KeyDown", InputEvent.IE_Pressed, _key =>
                {
                    if (_key.ToString() == "A")
                    {
                        IOToolkit.Key _slave = "OAxis_240";
                        ioDev1.SetDO(_slave, 1);
                        IOToolkit.Key _type = "OAxis_241";
                        ioDev1.SetDO(_type, 0x01);
                        IOToolkit.Key _addr = "OAxis_242";
                        ioDev1.SetDO(_addr, 1);
                        ioDev1.DOImmediate();
                    }
                    else if (_key.ToString() == "B")
                    {
                        IOToolkit.Key _slave = "OAxis_240";
                        ioDev1.SetDO(_slave, 1);
                        IOToolkit.Key _type = "OAxis_241";
                        ioDev1.SetDO(_type, 0x01);
                        IOToolkit.Key _addr = "OAxis_242";
                        ioDev1.SetDO(_addr, 0);
                        ioDev1.DOImmediate();
                    }
                });

                var ioDev2 = IODeviceController.GetIODevice($"extDev2");
                IOToolkit.Key _defaultWriteFunc = "OAxis_250"; ;
                ioDev2.SetDO(_defaultWriteFunc, 6);
                ioDev2.BindAction("KeyDown", InputEvent.IE_Pressed, _key =>
                {
                    if (_key.ToString() == "C")
                    {
                        ioDev2.SetDO(IOKeyCode.OAxis_00, 32);

                        //IOToolkit.Key _slave = "OAxis_240";
                        //ioDev2.SetDO(_slave, 1);
                        //IOToolkit.Key _type = "OAxis_241";
                        //ioDev2.SetDO(_type, 0x01);
                        //IOToolkit.Key _addr = "OAxis_242";
                        //ioDev2.SetDO(_addr, 1);
                        //ioDev2.DOImmediate();
                    }
                    else if (_key.ToString() == "D")
                    {
                        ioDev2.SetDO(IOKeyCode.OAxis_00, 0);
                        //IOToolkit.Key _slave = "OAxis_240";
                        //ioDev2.SetDO(_slave, 1);
                        //IOToolkit.Key _type = "OAxis_241";
                        //ioDev2.SetDO(_type, 0x01);
                        //IOToolkit.Key _addr = "OAxis_242";
                        //ioDev2.SetDO(_addr, 0);
                        //ioDev2.DOImmediate();
                    }
                });

                Observable.Interval(TimeSpan.FromMilliseconds(40))
                    .ObserveOn(RxApp.MainThreadScheduler)
                    .Subscribe(_ =>
                    {
                        // Debug.WriteLine("update.");
                        IODeviceController.Update();
                    }).DisposeWith(disposables);

                Disposable
                 .Create(() =>
                 {
                     IODeviceController.Unload();
                     Debug.WriteLine("Disposed...");
                 })
                 .DisposeWith(disposables);
            });
        }
    }
}
