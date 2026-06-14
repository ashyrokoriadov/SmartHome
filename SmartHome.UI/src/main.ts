import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { AppModule } from './app/app.module';

console.log('main: starting bootstrap');
platformBrowserDynamic().bootstrapModule(AppModule)
  .then(() => console.log('main: bootstrap succeeded'))
  .catch(err => {
    console.error('main: bootstrap failed', err);
    throw err;
  });