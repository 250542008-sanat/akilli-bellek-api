'use strict';

const API_BASE   = 'http://127.0.0.1:8000/api/v1';
const MAX_POINTS = 40;

const dom = {
    startBtn:         document.getElementById('startBtn'),
    stopBtn:          document.getElementById('stopBtn'),
    fixAllBtn:        document.getElementById('fixAllBtn'),
    clearBtn:         document.getElementById('clearBtn'),
    logs:             document.getElementById('logs'),
    autoScroll:       document.getElementById('autoScroll'),
    connectionBadge:  document.getElementById('connectionBadge'),
    badgeText:        document.querySelector('.badge-text'),
    sessionId:        document.getElementById('sessionId'),
    systemClock:      document.getElementById('systemClock'),
    chartOverlay:     document.getElementById('chartOverlay'),
    appName:          document.getElementById('appName'),
    pidInput:         document.getElementById('pidInput'),
    modeSelect:       document.getElementById('modeSelect'),
    thresholdInput:   document.getElementById('thresholdInput'),
    infoTool:         document.getElementById('infoTool'),
    infoStatus:       document.getElementById('infoStatus'),
    leakTableBody:    document.getElementById('leakTableBody'),
    leakCountBadge:   document.getElementById('leakCountBadge'),
    metricLeaks:      document.getElementById('metricLeaks'),
    metricLeaksDelta: document.getElementById('metricLeaksDelta'),
    metricFixed:      document.getElementById('metricFixed'),
    metricFixedDelta: document.getElementById('metricFixedDelta'),
    metricLeakedMem:  document.getElementById('metricLeakedMem'),
    metricMemDelta:   document.getElementById('metricMemDelta'),
    metricTime:       document.getElementById('metricTime'),
};

const state = {
    running:       false,
    sessionId:     null,
    startTime:     null,
    timerInterval: null,
    pollInterval:  null,
    leaks:         [],
    leakIdCounter: 0,
    totalLeakedKB: 0,
    fixedCount:    0,
    ramSamples:    [],
    leakSamples:   [],
    labels:        [],
};

const FUNCTIONS = [
    'malloc()', 'new[]', 'calloc()', 'realloc()',
    'operator new()', 'strdup()', 'memdup()', 'xmalloc()',
];
const FILES = [
    'VeriHavuzu.cpp', 'main.cpp', 'SenaroyA.cpp',
    'BellekYoneticisi.cpp', 'utils.cpp', 'parser.cpp',
];
const PRIORITIES = ['Yüksek', 'Orta', 'Düşük'];
const PRIORITY_WEIGHTS = [0.5, 0.35, 0.15];

function randomPriority() {
    const r = Math.random();
    if (r < PRIORITY_WEIGHTS[0]) return PRIORITIES[0];
    if (r < PRIORITY_WEIGHTS[0] + PRIORITY_WEIGHTS[1]) return PRIORITIES[1];
    return PRIORITIES[2];
}

function randomHex() {
    return '0x' + Math.floor(Math.random() * 0xFFFFFFFF)
        .toString(16).toUpperCase().padStart(8, '0');
}

function pick(arr) {
    return arr[Math.floor(Math.random() * arr.length)];
}

function startClock() {
    const tick = () => {
        dom.systemClock.textContent = new Date().toTimeString().slice(0, 8);
    };
    tick();
    setInterval(tick, 1000);
}

function startTimer() {
    state.startTime = Date.now();
    state.timerInterval = setInterval(() => {
        const e = Math.floor((Date.now() - state.startTime) / 1000);
        const m = String(Math.floor(e / 60)).padStart(2, '0');
        const s = String(e % 60).padStart(2, '0');
        dom.metricTime.textContent = `${m}:${s}`;
    }, 1000);
}

function stopTimer() { clearInterval(state.timerInterval); }

function ts() { return new Date().toTimeString().slice(0, 8); }

function log(msg, type = 'info') {
    const tags = { info: '[SYS]', ok: '[OK ]', error: '[ERR]', warn: '[WRN]', data: '[DAT]' };
    const el = document.createElement('span');
    el.className = `log-line log-${type}`;
    el.innerHTML = `${tags[type]} <span class="log-time">${ts()}</span> ${msg}`;
    dom.logs.appendChild(el);
    if (dom.autoScroll.checked) dom.logs.scrollTop = dom.logs.scrollHeight;
}

function setOnline(online) {
    dom.connectionBadge.classList.toggle('online', online);
    dom.badgeText.textContent = online ? 'Çevrimiçi' : 'Çevrimdışı';
    dom.infoStatus.textContent = online ? 'Aktif' : 'Beklemede';
}

let chart = null;

function initChart() {
    const ctx = document.getElementById('memoryChart').getContext('2d');
    chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: state.labels,
            datasets: [
                {
                    label: 'Kullanılan RAM (MB)',
                    data: state.ramSamples,
                    borderColor: '#38bdf8',
                    backgroundColor: 'rgba(56,189,248,0.07)',
                    fill: true, tension: 0.4, borderWidth: 2,
                    pointRadius: 3,
                    pointBackgroundColor: '#38bdf8',
                    pointBorderColor: '#080c14',
                    pointBorderWidth: 1.5,
                },
                {
                    label: 'Sızan Bellek (KB)',
                    data: state.leakSamples,
                    borderColor: '#ef4444',
                    backgroundColor: 'rgba(239,68,68,0.07)',
                    fill: true, tension: 0.4, borderWidth: 2,
                    pointRadius: 3,
                    pointBackgroundColor: '#ef4444',
                    pointBorderColor: '#080c14',
                    pointBorderWidth: 1.5,
                    yAxisID: 'y2',
                },
            ],
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: { duration: 300 },
            interaction: { intersect: false, mode: 'index' },
            plugins: {
                legend: { display: false },
                tooltip: {
                    backgroundColor: '#0d1422',
                    borderColor: 'rgba(56,189,248,0.3)',
                    borderWidth: 1,
                    titleColor: '#94a3b8',
                    bodyColor: '#f0f6ff',
                    titleFont: { family: 'JetBrains Mono', size: 11 },
                    bodyFont:  { family: 'JetBrains Mono', size: 12 },
                    padding: 12,
                },
            },
            scales: {
                x: {
                    ticks: { color: '#4b5a6e', font: { family: 'JetBrains Mono', size: 10 }, maxTicksLimit: 8 },
                    grid:  { color: 'rgba(56,189,248,0.05)' },
                },
                y: {
                    ticks: { color: '#4b5a6e', font: { family: 'JetBrains Mono', size: 10 }, callback: v => `${v} MB` },
                    grid:  { color: 'rgba(56,189,248,0.05)' },
                },
                y2: {
                    position: 'right',
                    ticks: { color: '#ef4444', font: { family: 'JetBrains Mono', size: 10 }, callback: v => `${v} KB` },
                    grid: { drawOnChartArea: false },
                },
            },
        },
    });
}

function priorityBadge(p) {
    const map = { 'Yüksek': 'badge-high', 'Orta': 'badge-medium', 'Düşük': 'badge-low' };
    return `<span class="badge ${map[p]}">${p}</span>`;
}

function renderTable() {
    if (state.leaks.length === 0) {
        dom.leakTableBody.innerHTML = `
            <tr class="empty-row">
                <td colspan="9">Henüz sızıntı tespit edilmedi</td>
            </tr>`;
        return;
    }

    dom.leakTableBody.innerHTML = state.leaks.map((l, i) => `
        <tr>
            <td style="color:var(--text-3)">${i + 1}</td>
            <td style="color:var(--accent);font-family:var(--mono)">${l.address}</td>
            <td>${l.size} KB</td>
            <td style="color:var(--text-1)">${l.func}</td>
            <td style="color:var(--text-2)">${l.file}</td>
            <td>${l.line}</td>
            <td>${l.fixed ? `<span class="badge badge-fixed">Giderildi</span>` : priorityBadge(l.priority)}</td>
            <td>${l.fixed
                ? `<span style="color:var(--success);font-size:11px">✓ Temiz</span>`
                : `<span style="color:var(--danger);font-size:11px">⚠ Aktif</span>`}
            </td>
            <td>
                <button class="btn-fix-inline" onclick="fixLeak(${l.id})" ${l.fixed ? 'disabled' : ''}>
                    ${l.fixed ? 'Giderildi' : 'Gider'}
                </button>
            </td>
        </tr>
    `).join('');

    dom.leakCountBadge.textContent =
        `${state.leaks.filter(l => !l.fixed).length} aktif sızıntı`;
}

window.fixLeak = function(id) {
    const leak = state.leaks.find(l => l.id === id);
    if (!leak || leak.fixed) return;
    leak.fixed = true;
    state.fixedCount++;
    state.totalLeakedKB = Math.max(0, state.totalLeakedKB - leak.size);

    dom.metricFixed.textContent      = state.fixedCount;
    dom.metricFixedDelta.textContent = `↓ ${state.fixedCount} giderildi`;
    dom.metricFixedDelta.className   = 'metric-delta down';
    dom.metricLeakedMem.textContent  = `${state.totalLeakedKB.toFixed(1)} KB`;

    log(`Sızıntı giderildi → ${leak.address} (${leak.size} KB, ${leak.func})`, 'ok');
    renderTable();
};

function fixAll() {
    const active = state.leaks.filter(l => !l.fixed);
    if (active.length === 0) return;
    active.forEach(l => { l.fixed = true; state.fixedCount++; });
    state.totalLeakedKB = 0;

    dom.metricFixed.textContent      = state.fixedCount;
    dom.metricFixedDelta.textContent = `↓ Tümü giderildi`;
    dom.metricFixedDelta.className   = 'metric-delta down';
    dom.metricLeakedMem.textContent  = '0 KB';

    log(`Tüm sızıntılar giderildi — ${active.length} adet temizlendi`, 'ok');
    renderTable();
}

function generateLeak() {
    const threshold = parseInt(dom.thresholdInput.value) || 64;
    const size = Math.floor(threshold * 0.2 + Math.random() * threshold * 1.5);
    const leak = {
        id:       state.leakIdCounter++,
        address:  randomHex(),
        size,
        func:     pick(FUNCTIONS),
        file:     pick(FILES),
        line:     Math.floor(Math.random() * 400) + 10,
        priority: randomPriority(),
        fixed:    false,
    };
    state.leaks.unshift(leak);
    if (state.leaks.length > 50) state.leaks.pop();
    state.totalLeakedKB += size;

    dom.metricLeaks.textContent      = state.leaks.length;
    dom.metricLeaksDelta.textContent = `↑ ${state.leaks.filter(l => !l.fixed).length} aktif`;
    dom.metricLeaksDelta.className   = 'metric-delta up';
    dom.metricLeakedMem.textContent  = `${state.totalLeakedKB.toFixed(1)} KB`;
    dom.metricMemDelta.textContent   = `↑ ${size} KB eklendi`;
    dom.metricMemDelta.className     = 'metric-delta up';

    log(`Sızıntı tespit edildi → ${leak.address} | ${size} KB | ${leak.func} (${leak.file}:${leak.line}) [${leak.priority}]`, 'warn');
    renderTable();
}

function updateChart() {
    const t    = new Date().toTimeString().slice(3, 8);
    const used = 400 + Math.random() * 150 + state.totalLeakedKB * 0.1;

    state.labels.push(t);
    state.ramSamples.push(parseFloat(used.toFixed(1)));
    state.leakSamples.push(parseFloat(state.totalLeakedKB.toFixed(1)));

    if (state.labels.length > MAX_POINTS) {
        state.labels.shift();
        state.ramSamples.shift();
        state.leakSamples.shift();
    }

    chart.update();
}

async function callBackend() {
    const payload = {
        uygulama_adi: dom.appName.value.trim() || 'Bellek_Test',
        pid:          parseInt(dom.pidInput.value) || 123,
        mod:          dom.modeSelect.value,
    };

    log(`İstek gönderiliyor → ${API_BASE}/analiz/baslat`, 'info');

    try {
        const res = await fetch(`${API_BASE}/analiz/baslat`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload),
        });
        if (!res.ok) throw new Error(`HTTP ${res.status}`);
        const data = await res.json();
        state.sessionId = data.oturum_id;
        dom.sessionId.textContent = state.sessionId;
        log(`Analiz başlatıldı — Oturum: ${state.sessionId}`, 'ok');
        setOnline(true);
    } catch (err) {
        log(`Backend erişilemez (${err.message}) — Demo moda geçildi`, 'warn');
        state.sessionId = 'DEMO-' + Math.random().toString(36).slice(2, 8).toUpperCase();
        dom.sessionId.textContent = state.sessionId;
        setOnline(false);
    }
}

async function onStart() {
    if (state.running) return;
    state.running = true;

    dom.startBtn.disabled  = true;
    dom.stopBtn.disabled   = false;
    dom.fixAllBtn.disabled = false;

    state.leaks         = [];
    state.leakIdCounter = 0;
    state.totalLeakedKB = 0;
    state.fixedCount    = 0;
    state.labels.length = state.ramSamples.length = state.leakSamples.length = 0;

    dom.metricLeaks.textContent     = '0';
    dom.metricFixed.textContent     = '0';
    dom.metricLeakedMem.textContent = '0 KB';
    dom.metricTime.textContent      = '00:00';

    chart.update();
    dom.chartOverlay.classList.add('hidden');
    renderTable();

    dom.infoTool.textContent =
        dom.modeSelect.options[dom.modeSelect.selectedIndex].text;

    await callBackend();
    startTimer();

    log(`Profilleme başladı — Araç: ${dom.modeSelect.value.toUpperCase()} | PID: ${dom.pidInput.value}`, 'data');

    updateChart();

    state.pollInterval = setInterval(() => {
        updateChart();
        if (Math.random() < 0.30) generateLeak();
    }, 2000);
}

function onStop() {
    if (!state.running) return;
    state.running = false;
    clearInterval(state.pollInterval);
    stopTimer();
    dom.startBtn.disabled  = false;
    dom.stopBtn.disabled   = true;
    dom.fixAllBtn.disabled = true;
    setOnline(false);
    dom.sessionId.textContent = '—';
    log(`Profilleme durduruldu — ${state.leaks.length} sızıntı, ${state.fixedCount} giderildi`, 'ok');
}

dom.startBtn.addEventListener('click', onStart);
dom.stopBtn.addEventListener('click', onStop);
dom.fixAllBtn.addEventListener('click', fixAll);
dom.clearBtn.addEventListener('click', () => {
    dom.logs.innerHTML = '';
    log('Günlük temizlendi', 'info');
});

startClock();
initChart();
log('Profilleme aracı hazır — analiz başlatmak için butona tıklayın', 'info');
