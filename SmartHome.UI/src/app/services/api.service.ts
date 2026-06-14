import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, map } from 'rxjs';

@Injectable({ providedIn: 'root' })
export class ApiService {
  private base = 'http://localhost:5000';

  constructor(private http: HttpClient) { }

  getHealth(): Observable<string> {
    // Zakładam istniejący endpoint GET /health lub GET /api/health. Dostosuj.
    return this.http.get(`${this.base}/api/Health`, { responseType: 'text' });
  }
}
