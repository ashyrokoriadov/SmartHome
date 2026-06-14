import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, map } from 'rxjs';

@Injectable({ providedIn: 'root' })
export class ApiService {
  // Use relative paths so Angular dev-server proxy (proxy.conf.json) handles CORS in development.
  private base = '';

  constructor(private http: HttpClient) { }

  getHealth(): Observable<string> {
    // Correct health-check endpoint on backend is /HealthCheck
    return this.http.get(`/HealthCheck`, { responseType: 'text' });
  }

  // Fetch battery range data using query params `start` and `end` (yyyy-MM-dd).
  // The API returns text/plain containing JSON, so request as text and parse.
  getBatteryRange(start: string, end: string): Observable<any[]> {
    return this.http.get(`/Battery/Range`, { params: { start, end }, responseType: 'text' })
      .pipe(map(text => {
        try { return JSON.parse(text); }
        catch { return [] as any[]; }
      }));
  }

  // Return raw text response for debugging (preserves original server body).
  getBatteryRangeRaw(start: string, end: string) {
    return this.http.get(`/Battery/Range`, { params: { start, end }, responseType: 'text' });
  }

  // Light sensor endpoints (same pattern as battery)
  getLightSensorRange(start: string, end: string): Observable<any[]> {
    return this.http.get(`/LightSensor/Range`, { params: { start, end }, responseType: 'text' })
      .pipe(map(text => {
        try { return JSON.parse(text); }
        catch { return [] as any[]; }
      }));
  }

  getLightSensorRangeRaw(start: string, end: string) {
    return this.http.get(`/LightSensor/Range`, { params: { start, end }, responseType: 'text' });
  }

  // Temperature endpoints
  getTemperatureRange(start: string, end: string): Observable<any[]> {
    return this.http.get(`/Temperature/Range`, { params: { start, end }, responseType: 'text' })
      .pipe(map(text => {
        try { return JSON.parse(text); }
        catch { return [] as any[]; }
      }));
  }

  getTemperatureRangeRaw(start: string, end: string) {
    return this.http.get(`/Temperature/Range`, { params: { start, end }, responseType: 'text' });
  }
}
