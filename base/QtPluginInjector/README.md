# QtPluginInjector

The modified version of qt platform plugins loads custom plugins provided the location and 
the public slot to call from the custom plugin's interface when a qt application instance is 
found on runtime of the Qt Application which uses this modified version of qt platform
plugin.

To keep it simple , it simply loads your cool qt plugins automatically without the need to
recompiling the Qt Application which needs to use your cool qt plugin.

For now this only works for qt plugins that extends qt itself , (i.e) no style plugins and other 
stuff but it will be supported later if possible.


**With this you can just ask your consumers to replace the platform plugin with this modified version 
along with your plugin and it just works!**


**Important**: This type of injection is only possible with Qt Application which uses shared Qt5 Framework.

# How it works

Qt Applications always(most typically) tends to use **qtxcb.so** in linux and other respective plugins for respective other platforms.
We simply modify this plugin at source and compile it with a modified code which looks for a configuration with 
which it loads and calls the custom plugin in question.

Now your plugin is wrapped with a class which uses QTimer in a different thread and waits for the construction 
of QCoreApplication or equivalent. If constructed then it makes all allocated memory as child to this 
QCoreApplication( so all memory is freed when the application is destroyed).

Thus your plugin can run and use the QCoreApplication instance too using ```qApp``` macro.

For now this injection is in prototype stage and therefore we are not documenting this.

# License

Only the patches are under the MIT License.   
Copyright (c) 2019 , The Future Shell Laboratory.
