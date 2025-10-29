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

                var ioDev1 = IODeviceController.GetIODevice($"extDev1");


                Enumerable.Range(0, 4)
                    .ToList().ForEach(_idx =>
                    {
                        ioDev1.BindAction($"PlayVideo_{_idx}", InputEvent.IE_Pressed, _key =>
                        {
                            if(_key == "A")
                            {
                                ioDev1.SetDO(IOKeyCode.OAxis_00, 1);
                                ioDev1.SetDO(IOKeyCode.OAxis_02, 2);
                            }
                            else if(_key == "B")
                            {
                                ioDev1.SetDO(IOKeyCode.OAxis_00, 0);
                                ioDev1.SetDO(IOKeyCode.OAxis_240, 1);
                                ioDev1.SetDO(IOKeyCode.OAxis_241, 1);
                                ioDev1.SetDO(IOKeyCode.OAxis_242, 1);
                            }
                            else if(_key == "C")
                            {
                                ioDev1.SetDO(IOKeyCode.OAxis_250, 64);
                            }

                            Debug.WriteLine("D1 " + _key + " Pressed");
                            ioDev1.SetDOOn("Output");
                        });

                        ioDev1.BindAction($"PlayVideo_{_idx}", InputEvent.IE_Released, _key =>
                        {
                            Debug.WriteLine("D1 " + _key + " Released");
                            
                            ioDev1.SetDOOff("Output");
                        });
                    });

                ioDev1.BindAxisKey(IOKeyCode.Axis_00, _val =>
                {
                    Debug.WriteLine(_val);
                });

                //Observable.Interval(TimeSpan.FromMilliseconds(500))
                //    .ObserveOn(RxApp.MainThreadScheduler)
                //    .Subscribe(_ =>
                //    {
                //        if (_ % 2 == 0)
                //        {
                //            Debug.WriteLine("Load...");
                //            IODeviceController.Load();
                //            var ioDev1 = IODeviceController.GetIODevice($"extDev1");

                //            Enumerable.Range(0, 4)
                //                .ToList().ForEach(_idx =>
                //                {
                //                    ioDev1.BindAction($"PlayVideo_{_idx}", InputEvent.IE_Pressed, _key =>
                //                    {
                //                        Debug.WriteLine("D1 " + _key + " Pressed");
                //                    });
                //                });

                //            ioDev1.BindAxisKey(IOKeyCode.Button_00, _val =>
                //            {
                //                Debug.WriteLine(_val);
                //            });
                //        }
                //        else
                //        {
                //            Debug.WriteLine("UnLoad...");
                //            IODeviceController.Unload();
                //        }
                //    }).DisposeWith(disposables);

                Observable.Interval(TimeSpan.FromMilliseconds(50))
                    .ObserveOn(RxApp.MainThreadScheduler)
                    .Subscribe(_ =>
                    {
                        //Debug.WriteLine("update -- .");
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
