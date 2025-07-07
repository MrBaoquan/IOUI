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
                            Debug.WriteLine("D1 " + _key + " Pressed");
                        });
                    });

                ioDev1.BindAxisKey(IOKeyCode.Button_00, _val =>
                {
                    Debug.WriteLine(_val);
                });

                Observable.Interval(TimeSpan.FromMilliseconds(40))
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
