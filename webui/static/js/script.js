// Tab Management
const tabs = document.querySelectorAll('.tab-btn');
const panels = document.querySelectorAll('.tab-panel');
const activeInput = document.getElementById('active_tab');

function activateTab(target) {
    // Reset all tabs
    tabs.forEach(b => {
        b.classList.remove('border-brand-500', 'text-brand-600');
        b.classList.add('border-transparent', 'text-slate-500');
    });

    // Hide all panels
    panels.forEach(p => p.classList.add('hidden'));

    // Activate target
    const btn = document.querySelector(`[data-tab="${target}"]`);
    if (btn) {
        btn.classList.remove('border-transparent', 'text-slate-500');
        btn.classList.add('border-brand-500', 'text-brand-600');
    }

    const panel = document.getElementById(target);
    if (panel) panel.classList.remove('hidden');

    if (activeInput) activeInput.value = target;

    // Sync select for mobile
    const select = document.getElementById('tabs');
    if (select) select.value = target;
}

tabs.forEach(btn => {
    btn.addEventListener('click', () => activateTab(btn.dataset.tab));
});

// Initialize
if (activeInput) {
    activateTab(activeInput.value || 'tab-file');
}

// Graph Editor Logic
const edges = [];

function renderEdges() {
    const list = document.getElementById('edge-list');
    list.innerHTML = '';

    if (edges.length === 0) {
        list.innerHTML = '<li class="px-4 py-3 text-sm text-slate-400 text-center italic">Aucune arête ajoutée</li>';
        return;
    }

    edges.forEach((e, idx) => {
        const li = document.createElement('li');
        li.className = "px-4 py-2 flex items-center justify-between hover:bg-slate-50 transition-colors";

        const content = document.createElement('div');
        content.className = "flex items-center gap-3";
        content.innerHTML = `
      <span class="flex h-6 w-6 items-center justify-center rounded-full bg-slate-100 text-xs font-medium text-slate-600">${idx + 1}</span>
      <span class="text-sm font-mono text-slate-700">${e.from} <span class="text-slate-400">→</span> ${e.to}</span>
      <span class="inline-flex items-center rounded-md bg-green-50 px-2 py-1 text-xs font-medium text-green-700 ring-1 ring-inset ring-green-600/20">${e.proba}</span>
    `;

        const btn = document.createElement('button');
        btn.innerHTML = '<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 6h18"/><path d="M19 6v14c0 1-1 2-2 2H7c-1 0-2-1-2-2V6"/><path d="M8 6V4c0-1 1-2 2-2h4c1 0 2 1 2 2v2"/></svg>';
        btn.className = 'text-slate-400 hover:text-red-600 transition-colors p-1';
        btn.onclick = () => { edges.splice(idx, 1); renderEdges(); };

        li.appendChild(content);
        li.appendChild(btn);
        list.appendChild(li);
    });
}

function addEdge() {
    const from = document.getElementById('edge-from').value;
    const to = document.getElementById('edge-to').value;
    const proba = document.getElementById('edge-proba').value;
    if (!from || !to || !proba) return;
    edges.push({ from, to, proba });
    renderEdges();

    // Clear inputs
    document.getElementById('edge-from').value = '';
    document.getElementById('edge-to').value = '';
    document.getElementById('edge-proba').value = '';
    document.getElementById('edge-from').focus();
}

function resetEdges() {
    edges.splice(0, edges.length);
    renderEdges();
    const area = document.querySelector('textarea[name="graph_text"]');
    if (area) area.value = '';
    const preview = document.getElementById('editor-preview');
    if (preview) preview.textContent = '';
}

function applyEdges() {
    const n = document.getElementById('vertex-count').value || '';
    const payload = { n: n, edges: edges };
    const hidden = document.getElementById('editor_payload');
    if (hidden) hidden.value = JSON.stringify(payload);

    const txt = (n ? n + "\n" : "") + edges.map(e => `${e.from} ${e.to} ${e.proba}`).join("\n");

    // Update manual text area if exists
    const area = document.querySelector('textarea[name="graph_text"]');
    if (area) {
        area.value = txt;
    }

    const preview = document.getElementById('editor-preview');
    if (preview) preview.textContent = txt || '(Vide)';

    // Visual feedback
    const btn = document.querySelector('button[onclick="applyEdges()"]');
    const originalText = btn.innerHTML;
    btn.innerHTML = '<span>✓ Appliqué</span>';
    btn.classList.add('bg-emerald-700');
    setTimeout(() => {
        btn.innerHTML = originalText;
        btn.classList.remove('bg-emerald-700');
    }, 2000);
}
