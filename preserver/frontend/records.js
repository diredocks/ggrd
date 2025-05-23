// Function to convert Unix timestamp to local datetime string
function formatTimestamp(timestamp) {
    return new Date(timestamp * 1000).toLocaleString();
}

// Function to convert datetime-local input to Unix timestamp
function datetimeToTimestamp(datetimeStr) {
    if (!datetimeStr) return null;
    return Math.floor(new Date(datetimeStr).getTime() / 1000);
}

// Function to create a face card element
function createFaceCard(face) {
    const card = document.createElement('div');
    card.className = 'face-card';
    
    const frameImg = document.createElement('img');
    frameImg.src = `/api/image/frame?id=${face.id}`;
    frameImg.alt = `Frame for ${face.label || 'Unknown'}`;
    
    const info = document.createElement('div');
    info.className = 'face-info';
    
    const name = document.createElement('h3');
    name.textContent = face.label || 'Unknown';
    
    const firstSeen = document.createElement('p');
    firstSeen.textContent = `First seen: ${formatTimestamp(face.first_seen)}`;
    
    const lastSeen = document.createElement('p');
    lastSeen.textContent = `Last seen: ${formatTimestamp(face.last_seen)}`;
    
    info.appendChild(name);
    info.appendChild(firstSeen);
    info.appendChild(lastSeen);
    
    card.appendChild(frameImg);
    card.appendChild(info);
    
    return card;
}

// Function to fetch and display face records
async function fetchFaceRecords(time = null, label = null) {
    try {
        let url = '/api/face';
        const params = new URLSearchParams();
        
        if (time) params.append('time', time);
        if (label) params.append('label', label);
        
        if (params.toString()) {
            url += '?' + params.toString();
        }
        
        const response = await fetch(url);
        if (!response.ok) throw new Error('Failed to fetch face records');
        
        const faces = await response.json();
        const faceGrid = document.getElementById('faceGrid');
        faceGrid.innerHTML = ''; // Clear existing cards
        
        faces.forEach(face => {
            const card = createFaceCard(face);
            faceGrid.appendChild(card);
        });
    } catch (error) {
        console.error('Error fetching face records:', error);
        alert('Failed to load face records. Please try again.');
    }
}

// Function to apply filters
window.applyFilters = function() {
    const nameFilter = document.getElementById('nameFilter').value;
    const timeFilter = document.getElementById('timeFilter').value;
    
    const timestamp = datetimeToTimestamp(timeFilter);
    fetchFaceRecords(timestamp, nameFilter);
};

// Initial load
document.addEventListener('DOMContentLoaded', () => {
    fetchFaceRecords();
}); 