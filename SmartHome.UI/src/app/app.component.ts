import { Component, OnInit } from '@angular/core';
import { ApiService } from './services/api.service';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html'
})
export class AppComponent implements OnInit {
  title = 'SmartHome UI';
  health = '';
  rangeData: any[] = [];
  rangeError = '';
  rangeLoading = false;
  rangeRaw: string | null = null;
  rangeParseError: string | null = null;

  lightData: any[] = [];
  lightError = '';
  lightLoading = false;
  lightRaw: string | null = null;
  lightParseError: string | null = null;

  tempData: any[] = [];
  tempError = '';
  tempLoading = false;
  tempRaw: string | null = null;
  tempParseError: string | null = null;

  // Gauges for battery (display first record)
  displayedVoltage = 0;
  displayedCurrent = 0;
  voltageAngle = 0; // 0-180
  currentAngle = 0; // 0-180

  constructor(private api: ApiService) { }

  ngOnInit(): void {
    console.log('AppComponent: ngOnInit');
    const el = document.getElementById('app-loading');
    if (el) el.remove();
  }

  checkHealth() {
    this.api.getHealth().subscribe({
      next: v => this.health = v,
      error: e => this.health = 'Error: ' + (e?.message || e)
    });
  }

  getRangeT7() {
    this.rangeData = [];
    this.rangeError = '';
    this.rangeLoading = true;

    const end = new Date();
    const start = new Date(end);
    start.setDate(end.getDate() - 7);
    const fmt = (d: Date) => d.toISOString().slice(0, 10);

    this.api.getBatteryRangeRaw(fmt(start), fmt(end)).subscribe({
      next: text => {
        this.rangeRaw = text;
        try {
          const parsed = JSON.parse(text);
          this.rangeData = parsed || [];
          this.rangeParseError = null;
        } catch (err: any) {
          this.rangeParseError = 'Parse error: ' + (err?.message || err);
          this.rangeData = [];
        }
        // update gauges based on first value
        if (this.rangeData && this.rangeData.length) {
          const first = this.rangeData[0] as any;
          const v = Number(first?.voltage) || 0;
          const c = Number(first?.current) || 0;
          this.displayedVoltage = Math.max(0, Math.min(15, v));
          this.displayedCurrent = Math.max(0, Math.min(10, c));
          this.voltageAngle = (this.displayedVoltage / 15) * 180;
          this.currentAngle = (this.displayedCurrent / 10) * 180;
        } else {
          this.displayedVoltage = 0;
          this.displayedCurrent = 0;
          this.voltageAngle = 0;
          this.currentAngle = 0;
        }

        this.rangeLoading = false;
      },
      error: e => { this.rangeError = 'Error: ' + (e?.message || e); this.rangeLoading = false; }
    });
  }

  getLightRangeT7() {
    this.lightData = [];
    this.lightError = '';
    this.lightLoading = true;

    const end = new Date();
    const start = new Date(end);
    start.setDate(end.getDate() - 7);
    const fmt = (d: Date) => d.toISOString().slice(0, 10);

    this.api.getLightSensorRangeRaw(fmt(start), fmt(end)).subscribe({
      next: text => {
        this.lightRaw = text;
        try {
          const parsed = JSON.parse(text);
          this.lightData = parsed || [];
          this.lightParseError = null;
        } catch (err: any) {
          this.lightParseError = 'Parse error: ' + (err?.message || err);
          this.lightData = [];
        }
        this.lightLoading = false;
      },
      error: e => { this.lightError = 'Error: ' + (e?.message || e); this.lightLoading = false; }
    });
  }

  getTemperatureRangeT7() {
    this.tempData = [];
    this.tempError = '';
    this.tempLoading = true;

    const end = new Date();
    const start = new Date(end);
    start.setDate(end.getDate() - 7);
    const fmt = (d: Date) => d.toISOString().slice(0, 10);

    this.api.getTemperatureRangeRaw(fmt(start), fmt(end)).subscribe({
      next: text => {
        this.tempRaw = text;
        try {
          const parsed = JSON.parse(text);
          this.tempData = parsed || [];
          this.tempParseError = null;
        } catch (err: any) {
          this.tempParseError = 'Parse error: ' + (err?.message || err);
          this.tempData = [];
        }
        this.tempLoading = false;
      },
      error: e => { this.tempError = 'Error: ' + (e?.message || e); this.tempLoading = false; }
    });
  }
}
