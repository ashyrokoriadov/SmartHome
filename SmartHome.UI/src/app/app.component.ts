import { Component } from '@angular/core';
import { ApiService } from './services/api.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html'
})
export class AppComponent {
  title = 'SmartHome UI';
  health = '';

  constructor(private api: ApiService) { }

  checkHealth() {
    this.api.getHealth().subscribe({
      next: v => this.health = v,
      error: e => this.health = 'Error: ' + (e?.message || e)
    });
  }
}
