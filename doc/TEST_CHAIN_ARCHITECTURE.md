# Test Chain (Battery) Architecture Plan

**Date:** October 17, 2025
**Status:** Planning Phase

## Overview

This document describes the architecture for implementing "Test Chains" (also called "Batteries") - sequential execution of multiple PEBL tests, instruction pages, and consent forms that run one after another using the same participant ID.

## Motivation

Currently, the PEBL web platform supports individual tests within a "Study". However, real research often requires:
- Running multiple tests in sequence
- Repeating tests with different parameter settings
- Randomizing test order (within-subjects designs)
- Adding consent forms and instruction pages
- Tracking progress through a multi-test battery
- Integration with external platforms (Prolific, MTurk, Qualtrics)

The native PEBL version has "chain" functionality that we need to replicate and enhance for the web platform.

## Key Concepts

### Test Chain vs. Study

**Study** (existing system):
- Collection of individual tests
- Each test accessed via separate URL
- No sequential relationship between tests
- Token-based access control

**Test Chain** (new system):
- Ordered sequence of items (tests, pages, etc.)
- Single URL launches entire sequence
- Progress tracked through battery
- Can be standalone or linked to a Study
- Designed for external platform integration

### Chain Items

A chain consists of ordered items:
1. **Tests** - PEBL battery tests with optional parameter variants
2. **Instruction Pages** - HTML pages with custom content
3. **Consent Forms** - IRB consent with required acknowledgment
4. **Randomization Groups** - Subsets of items with randomized order

## Database Schema

### test_chains Table

Stores basic chain metadata:

```sql
CREATE TABLE test_chains (
    chain_id TEXT PRIMARY KEY,           -- e.g., 'BATTERY_ABC123'
    chain_name TEXT NOT NULL,            -- e.g., 'Memory Study Battery'
    researcher_email TEXT,                -- Owner (optional - can be shared)
    description TEXT,                     -- Purpose, IRB info, etc.
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    modified_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    active INTEGER DEFAULT 1              -- Can be deactivated
);
```

### chain_items Table

Defines the sequence of items in a chain:

```sql
CREATE TABLE chain_items (
    item_id INTEGER PRIMARY KEY AUTOINCREMENT,
    chain_id TEXT NOT NULL,
    sequence_order INTEGER NOT NULL,      -- Order in chain (1, 2, 3...)
    item_type TEXT NOT NULL,              -- 'test', 'instruction', 'consent',
                                          -- 'randomize_start', 'randomize_end'

    -- For test items
    test_name TEXT,                       -- e.g., 'corsi', 'stroop'
    param_variant TEXT,                   -- e.g., 'fast', 'slow', or NULL for default

    -- For instruction/consent items
    content TEXT,                         -- HTML content
    title TEXT,                           -- Page title
    require_acknowledgment INTEGER,       -- 1 for consent forms

    -- For randomization
    randomize_group_id INTEGER,           -- Group items to randomize together

    -- Timing
    min_duration_seconds INTEGER,         -- Optional minimum time on page

    FOREIGN KEY (chain_id) REFERENCES test_chains(chain_id) ON DELETE CASCADE
);
```

**Item Types:**
- `test` - A PEBL battery test
- `instruction` - Static instruction page
- `consent` - Consent form requiring acknowledgment
- `randomize_start` - Marks beginning of randomization group
- `randomize_end` - Marks end of randomization group

### chain_sessions Table

Tracks participant sessions through a chain:

```sql
CREATE TABLE chain_sessions (
    session_id TEXT PRIMARY KEY,          -- UUID
    chain_id TEXT NOT NULL,
    participant_id TEXT NOT NULL,

    -- Timing
    started_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    completed_at DATETIME,
    last_activity_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    -- Progress
    current_item_id INTEGER,              -- Current position in chain
    randomized_order TEXT,                -- JSON array of item_ids (after randomization)

    -- Status
    status TEXT DEFAULT 'in_progress',    -- 'in_progress', 'completed', 'abandoned'

    -- External platform integration
    completion_code TEXT,                 -- For Prolific, MTurk, etc.
    return_url TEXT,                      -- Where to redirect on completion

    -- Optional study linkage
    study_token TEXT,                     -- Link to existing Study (optional)

    FOREIGN KEY (chain_id) REFERENCES test_chains(chain_id),
    FOREIGN KEY (study_token) REFERENCES tokens(token_id)
);
```

### chain_progress Table

Detailed progress through each item:

```sql
CREATE TABLE chain_progress (
    progress_id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id TEXT NOT NULL,
    item_id INTEGER NOT NULL,

    -- Timing
    started_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    completed_at DATETIME,

    -- Status
    status TEXT DEFAULT 'in_progress',    -- 'in_progress', 'completed', 'skipped'

    -- Metadata
    attempts INTEGER DEFAULT 1,           -- If item allows retries
    notes TEXT,                           -- Error messages, etc.

    FOREIGN KEY (session_id) REFERENCES chain_sessions(session_id) ON DELETE CASCADE,
    FOREIGN KEY (item_id) REFERENCES chain_items(item_id)
);
```

## Critical Technical Challenge: Test Exit Mechanism

**Problem:** Currently, when an Emscripten PEBL test completes, it just displays the final screen indefinitely. We need a way for the test to signal completion so the launcher can proceed to the next item.

### Implemented Solution: Custom Event Dispatch (AUTOMATIC)

**Status:** ✅ IMPLEMENTED

The test completion mechanism is now **fully automatic** - no script changes needed! PEBL automatically signals completion when the program exits (successfully or with errors).

**C++ Implementation:**

In `src/apps/PEBL.cpp`, added automatic completion signaling:

```cpp
#ifdef PEBL_EMSCRIPTEN
void SignalTestComplete(const char* status = "completed") {
    EM_ASM({
        var event = new CustomEvent('peblTestComplete', {
            detail: {
                status: UTF8ToString($0),
                timestamp: Date.now()
            }
        });
        document.dispatchEvent(event);
        console.log('PEBL test completed with status:', UTF8ToString($0));
    }, status);
}
#else
inline void SignalTestComplete(const char* status = "completed") {
    // No-op on native builds
}
#endif
```

**Automatic Hooks Added:**

1. **Normal completion** (`PEBL.cpp:780`):
   ```cpp
   SignalTestComplete("completed");
   ```

2. **Error handling** (`PError.cpp:96`):
   ```cpp
   SignalTestComplete("error");
   ```

3. **Signal capture** (`PEBL.cpp:837`):
   ```cpp
   SignalTestComplete("signal");
   ```

**JavaScript Side** - Chain launcher listens for event:

```javascript
document.addEventListener('peblTestComplete', function(e) {
    const status = e.detail.status;  // "completed", "error", "signal"
    const timestamp = e.detail.timestamp;

    console.log('Test finished:', status, 'at', new Date(timestamp));

    // Record completion in database with status
    recordItemCompletion(currentItemId, status);

    // Proceed to next item
    proceedToNextItem();
});
```

**Benefits:**

1. ✅ **Zero script changes** - Works with all existing tests automatically
2. ✅ **Captures all exit scenarios** - Normal completion, errors, signals
3. ✅ **Status information** - Know if test completed successfully or had issues
4. ✅ **Immediate** - No polling, event fires instantly
5. ✅ **Reliable** - No filesystem sync issues
6. ✅ **Clean architecture** - Exit handling in centralized locations

## Chain Launcher Architecture

### URL Format

```
http://localhost:8080/chain-launcher.html?chain={CHAIN_ID}&participant={PARTICIPANT_ID}[&return_url={URL}]
```

**Parameters:**
- `chain` - Chain ID (required)
- `participant` - Participant identifier (required)
- `return_url` - URL to redirect after completion (optional, for external platforms)
- `lang` - Language code (optional, default: 'en')

**Example for Prolific:**
```
http://localhost:8080/chain-launcher.html?chain=BATTERY_ABC123&participant={{%PROLIFIC_PID%}}&return_url=https://app.prolific.co/submissions/complete?cc=COMPLETION123
```

### Launcher Flow

```
1. Parse URL parameters
2. Initialize session
   ├─ Check if session exists (resume)
   └─ Create new session
3. Fetch chain configuration
   ├─ Get all chain items
   └─ Apply randomization (if first visit)
4. Load current item
   ├─ If test: Launch PEBL with parameters
   ├─ If instruction: Show HTML page
   └─ If consent: Show with acknowledgment
5. Wait for completion signal
   ├─ Test: Poll for filesystem flag
   ├─ Instruction: Wait for Continue button
   └─ Consent: Wait for acknowledgment checkbox + Continue
6. Update progress in database
   └─ Mark item complete, update timestamps
7. Check if chain complete
   ├─ Yes: Show completion page, redirect if return_url
   └─ No: Go to step 4 with next item
```

### State Management

The launcher maintains state in JavaScript:
```javascript
const chainState = {
    sessionId: 'uuid-here',
    chainId: 'BATTERY_ABC123',
    participantId: 'P001',
    currentItemIndex: 0,
    itemSequence: [1, 3, 2, 4, 5],  // After randomization
    completedItems: [1, 3],
    returnUrl: 'https://...'
};
```

## API Endpoints

### Chain Configuration API

**GET /api/chain_config.php?chain={CHAIN_ID}**

Returns chain structure:
```json
{
    "chain_id": "BATTERY_ABC123",
    "chain_name": "Memory Study Battery",
    "description": "Three memory tests",
    "items": [
        {
            "item_id": 1,
            "sequence_order": 1,
            "item_type": "consent",
            "title": "Informed Consent",
            "content": "<h1>Consent Form</h1>...",
            "require_acknowledgment": 1
        },
        {
            "item_id": 2,
            "sequence_order": 2,
            "item_type": "instruction",
            "title": "Study Instructions",
            "content": "<p>You will complete 3 tests...</p>"
        },
        {
            "item_id": 3,
            "sequence_order": 3,
            "item_type": "randomize_start",
            "randomize_group_id": 1
        },
        {
            "item_id": 4,
            "sequence_order": 4,
            "item_type": "test",
            "test_name": "corsi",
            "param_variant": null,
            "randomize_group_id": 1
        },
        {
            "item_id": 5,
            "sequence_order": 5,
            "item_type": "test",
            "test_name": "stroop",
            "param_variant": "fast",
            "randomize_group_id": 1
        },
        {
            "item_id": 6,
            "sequence_order": 6,
            "item_type": "randomize_end",
            "randomize_group_id": 1
        },
        {
            "item_id": 7,
            "sequence_order": 7,
            "item_type": "instruction",
            "title": "Thank You",
            "content": "<p>Study complete!</p>"
        }
    ]
}
```

### Chain Progress API

**POST /api/chain_progress.php**

#### Start/Resume Session
```json
{
    "action": "start_session",
    "chain_id": "BATTERY_ABC123",
    "participant_id": "P001",
    "return_url": "https://..."
}
```

Returns:
```json
{
    "session_id": "uuid-here",
    "current_item_id": 1,
    "randomized_order": [1, 2, 3, 5, 4, 6, 7],
    "completed_items": []
}
```

#### Complete Item
```json
{
    "action": "complete_item",
    "session_id": "uuid-here",
    "item_id": 1
}
```

Returns:
```json
{
    "success": true,
    "next_item_id": 2,
    "chain_complete": false
}
```

#### Get Session Status
```json
{
    "action": "get_session",
    "session_id": "uuid-here"
}
```

Returns current session state.

#### Complete Chain
```json
{
    "action": "complete_session",
    "session_id": "uuid-here"
}
```

Returns:
```json
{
    "success": true,
    "completion_code": "ABC123XYZ",
    "return_url": "https://..."
}
```

## Randomization Logic

### Randomization Groups

Items can be grouped for randomization:

```
1. Consent Form
2. Instructions
3. [Randomize Start - Group 1]
   4. Corsi
   5. Stroop
   6. N-Back
7. [Randomize End - Group 1]
8. Corsi (repeat with different params)
9. Thank You
```

When session starts, items 4-6 are shuffled, e.g., might become: 5, 4, 6

### Implementation

```php
function applyRandomization($chainItems, $sessionId) {
    $randomGroups = [];
    $result = [];

    foreach ($chainItems as $item) {
        if ($item['randomize_group_id']) {
            $randomGroups[$item['randomize_group_id']][] = $item;
        } else {
            $result[] = $item;
        }
    }

    // Shuffle each group
    foreach ($randomGroups as $groupId => &$items) {
        shuffle($items);
    }

    // Reconstruct full sequence
    $finalSequence = [];
    foreach ($chainItems as $item) {
        if ($item['item_type'] === 'randomize_start') {
            $groupId = $item['randomize_group_id'];
            // Insert shuffled items
            foreach ($randomGroups[$groupId] as $randomItem) {
                $finalSequence[] = $randomItem;
            }
        } elseif ($item['item_type'] !== 'randomize_end' && !$item['randomize_group_id']) {
            $finalSequence[] = $item;
        }
    }

    // Save randomized order to session
    $itemIds = array_map(fn($i) => $i['item_id'], $finalSequence);
    saveRandomizedOrder($sessionId, $itemIds);

    return $finalSequence;
}
```

## User Interface Components

### 1. Create Chain Builder (`create_chain.php`)

**Features:**
- Form to create new chain
- Chain name and description
- Add/remove items
- Drag-and-drop reordering
- Configure each item:
  - Tests: Select test, choose parameter variant
  - Instructions: HTML editor
  - Consent: HTML editor + acknowledgment option
- Define randomization groups
- Save/preview chain

**UI Mockup:**
```
┌─────────────────────────────────────────────────────────┐
│ Create Test Chain                                       │
├─────────────────────────────────────────────────────────┤
│ Chain Name: [Memory Battery                          ] │
│ Description: [Three memory tasks with randomization  ] │
│                                                         │
│ Chain Items:                    [+ Add Item ▼]         │
│ ┌─────────────────────────────────────────────────────┐│
│ │ 1. ☰ Consent Form              [Edit] [↑] [↓] [×]  ││
│ │ 2. ☰ Instructions              [Edit] [↑] [↓] [×]  ││
│ │ 3. ☰ 🔀 Randomize Start (Group 1)     [↑] [↓] [×]  ││
│ │    4. ☰ Corsi - Default        [Edit] [↑] [↓] [×]  ││
│ │    5. ☰ Stroop - Fast          [Edit] [↑] [↓] [×]  ││
│ │    6. ☰ N-Back - Hard          [Edit] [↑] [↓] [×]  ││
│ │ 7. ☰ 🔀 Randomize End (Group 1)       [↑] [↓] [×]  ││
│ │ 8. ☰ Corsi - Fast              [Edit] [↑] [↓] [×]  ││
│ │ 9. ☰ Thank You Page            [Edit] [↑] [↓] [×]  ││
│ └─────────────────────────────────────────────────────┘│
│                                                         │
│ [Save Chain] [Preview] [Cancel]                        │
└─────────────────────────────────────────────────────────┘
```

### 2. Manage Chains (`manage_chains.php`)

**Features:**
- List all chains
- Search/filter chains
- View chain details
- Edit chain
- Delete chain (with confirmation)
- Generate participant URLs
- View session analytics

**Chain Card Display:**
```
┌─────────────────────────────────────────────────────────┐
│ Memory Battery                            [Active ✓]   │
│ BATTERY_ABC123                                          │
├─────────────────────────────────────────────────────────┤
│ Description: Three memory tests with randomization     │
│                                                         │
│ Tests: Corsi, Stroop, N-Back                           │
│ Total Items: 9                                          │
│ Est. Duration: ~25 minutes                             │
│                                                         │
│ Sessions Started: 47                                    │
│ Sessions Completed: 42 (89%)                           │
│ Sessions Abandoned: 5 (11%)                            │
│                                                         │
│ Participant URL:                                        │
│ [http://localhost:8080/chain-launcher.html?chain...   ]│
│ [📋 Copy] [📱 QR Code]                                 │
│                                                         │
│ [✏️ Edit] [📊 Analytics] [🗑️ Delete]                   │
└─────────────────────────────────────────────────────────┘
```

### 3. Chain Analytics Dashboard (`chain_analytics.php?chain={ID}`)

**Features:**
- Overall completion rate
- Drop-off points (which items cause abandonment)
- Average time per item
- Time series of completions
- Individual session list
- Export data

### 4. Item Editors

**Instruction/Consent Editor:**
- Rich text HTML editor (TinyMCE or similar)
- Preview mode
- Template variables: `{PARTICIPANT_ID}`, `{CHAIN_NAME}`, etc.

**Test Item Editor:**
- Select test from available tests
- Choose parameter variant from dropdown
- Preview parameter values

## Content Page Rendering

### Instruction Pages

Simple HTML renderer with continue button:

```html
<!DOCTYPE html>
<html>
<head>
    <title>Instructions</title>
    <style>
        .instruction-container {
            max-width: 800px;
            margin: 50px auto;
            padding: 30px;
            font-family: Arial, sans-serif;
        }
        .continue-btn {
            background: #007bff;
            color: white;
            padding: 15px 40px;
            border: none;
            border-radius: 5px;
            font-size: 1.1em;
            cursor: pointer;
            margin-top: 30px;
        }
    </style>
</head>
<body>
    <div class="instruction-container">
        <div id="content">
            <!-- HTML content from database -->
        </div>
        <button class="continue-btn" onclick="continueChain()">
            Continue
        </button>
    </div>

    <script>
        function continueChain() {
            // Signal completion to parent (chain-launcher.html)
            window.parent.postMessage({
                type: 'item_complete',
                itemId: <?= $itemId ?>
            }, '*');
        }
    </script>
</body>
</html>
```

### Consent Forms

Similar but with required checkbox:

```html
<div class="consent-container">
    <div id="content">
        <!-- Consent form HTML -->
    </div>

    <label style="font-size: 1.1em; margin-top: 20px;">
        <input type="checkbox" id="consent-check" required>
        I have read and agree to the above consent form
    </label>

    <button class="continue-btn" id="continue-btn" disabled onclick="continueChain()">
        Continue
    </button>
</div>

<script>
    document.getElementById('consent-check').addEventListener('change', function() {
        document.getElementById('continue-btn').disabled = !this.checked;
    });
</script>
```

## External Platform Integration

### Prolific Integration

**URL Format:**
```
http://localhost:8080/chain-launcher.html?chain=BATTERY_ABC123&participant={{%PROLIFIC_PID%}}&return_url=https://app.prolific.co/submissions/complete?cc=COMPLETION_CODE
```

When chain completes:
1. Mark session as complete
2. Generate unique completion code (or use predefined)
3. Redirect to return_url with completion code

### Amazon MTurk Integration

Similar approach with MTurk survey codes.

### Qualtrics Integration

Qualtrics can redirect to external URL, then back to survey:
1. Qualtrics redirects to chain launcher
2. Chain completes
3. Redirect back to Qualtrics with completion status

## Implementation Phases

### Phase 1: Core Infrastructure (Week 1-2)

**Critical Path:**
1. Implement test exit mechanism
   - Add `SignalTestComplete()` to PEBL library
   - Update test templates to call at end
   - Test in 2-3 battery tests
2. Create database tables and migration script
3. Build basic chain launcher HTML
   - URL parsing
   - Session management
   - Item loading logic
   - Test completion detection

**Deliverable:** Can launch a chain with 2 tests in sequence

### Phase 2: Chain Management UI (Week 2-3)

4. Create chain configuration API (`api/chain_config.php`)
5. Create chain progress API (`api/chain_progress.php`)
6. Build create chain page (`create_chain.php`)
   - Basic form (no drag-drop yet)
   - Add tests with parameter selection
7. Build manage chains page (`manage_chains.php`)
   - List chains
   - Generate URLs

**Deliverable:** Can create and manage chains via web UI

### Phase 3: Content Pages (Week 3-4)

8. Build instruction page renderer
9. Build consent page renderer with acknowledgment
10. Add HTML editor to chain builder
11. Test with real consent form

**Deliverable:** Can include instruction/consent pages in chains

### Phase 4: Randomization (Week 4-5)

12. Implement randomization logic in API
13. Add randomization group UI to chain builder
14. Test randomization with multiple sessions

**Deliverable:** Randomized test order works correctly

### Phase 5: Analytics & Polish (Week 5-6)

15. Build analytics dashboard
16. Add drag-and-drop reordering to chain builder
17. Add session recovery (resume incomplete chains)
18. External platform integration (completion codes, etc.)
19. Documentation for researchers

**Deliverable:** Production-ready chain system

## Testing Strategy

### Unit Tests
- Randomization algorithm
- Session state management
- Item sequencing logic

### Integration Tests
- Complete 2-test chain
- Resume interrupted session
- Randomization produces different orders
- External platform redirect

### User Acceptance Tests
- Researcher creates chain
- Participant completes chain
- Data uploads correctly
- Analytics show accurate stats

## Migration from Existing System

**Backwards Compatibility:**
- Keep existing Study system unchanged
- Studies can optionally link to a Chain
- Individual test URLs still work

**Data Migration:**
- Existing sessions remain in `sessions` table
- New chain sessions in `chain_sessions` table
- No migration needed for existing data

## Open Questions

1. **How to handle test failures?**
   - Option A: Allow retry
   - Option B: Skip and continue
   - Option C: End chain

2. **Should chains be shareable between researchers?**
   - Public chain library?
   - Copy/clone functionality?

3. **How to handle very long chains?**
   - Save/resume across days?
   - Send reminder emails?

4. **Parameter inheritance?**
   - Should some parameters be chain-level (affect all tests)?
   - E.g., language, difficulty level

5. **Quota management?**
   - Limit number of sessions per chain?
   - Stop accepting participants after N completions?

## Future Enhancements

- **Conditional branching**: Skip items based on previous performance
- **Adaptive testing**: Adjust difficulty based on results
- **Time limits**: Maximum time per item or entire chain
- **Break pages**: Scheduled breaks between tests
- **Multi-session chains**: Complete over multiple days
- **Email reminders**: For incomplete sessions
- **Mobile app support**: Native launcher for mobile devices

## References

- Native PEBL chain implementation: (to be documented)
- Emscripten filesystem API: https://emscripten.org/docs/api_reference/Filesystem-API.html
- Prolific integration guide: https://docs.prolific.co/
- PEBL manual: `/doc/PEBL_v2.1_Manual.pdf`

---

**Next Step:** Implement test exit mechanism (Phase 1, Step 1)
