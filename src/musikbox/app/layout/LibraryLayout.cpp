#include "stdafx.h"

#include <cursespp/Colors.h>
#include <cursespp/Screen.h>

#include <core/library/LocalLibraryConstants.h>

#include "LibraryLayout.h"

using namespace musik::core::library::constants;

#define CATEGORY_WIDTH 25

#ifdef WIN32
    #define TRANSPORT_HEIGHT 3
#else
    #define TRANSPORT_HEIGHT 2
#endif

#define DEFAULT_CATEGORY constants::Track::ALBUM_ID

using namespace musik::core;
using namespace musik::core::audio;
using namespace musik::core::library;
using namespace musik::box;
using namespace cursespp;

LibraryLayout::LibraryLayout(PlaybackService& playback, LibraryPtr library)
: LayoutBase()
, playback(playback)
, transport(playback.GetTransport()) {
    this->library = library;
    this->InitializeWindows();
}

LibraryLayout::~LibraryLayout() {

}

void LibraryLayout::Layout() {
    this->SetSize(Screen::GetWidth(), Screen::GetHeight());
    this->SetPosition(0, 0);

    this->categoryList->MoveAndResize(
        0,
        0,
        CATEGORY_WIDTH,
        this->GetHeight() - TRANSPORT_HEIGHT);

    this->categoryList->SetFocusOrder(0);

    this->trackList->MoveAndResize(
        CATEGORY_WIDTH,
        0,
        this->GetWidth() - CATEGORY_WIDTH,
        this->GetHeight() - TRANSPORT_HEIGHT);

    this->trackList->SetFocusOrder(1);

    this->transportView->MoveAndResize(
        1,
        this->GetHeight() - TRANSPORT_HEIGHT,
        this->GetWidth() - 2,
        TRANSPORT_HEIGHT);

    this->transportView->Update();
}

void LibraryLayout::InitializeWindows() {
    this->categoryList.reset(new CategoryListView(this->library, DEFAULT_CATEGORY));
    this->trackList.reset(new TrackListView(this->playback, this->library));
    this->transportView.reset(new TransportWindow(this->library, this->transport));

    this->AddWindow(this->categoryList);
    this->AddWindow(this->trackList);
    this->AddWindow(this->transportView);

    this->categoryList->SelectionChanged.connect(
        this, &LibraryLayout::OnCategoryViewSelectionChanged);

    this->categoryList->Invalidated.connect(
        this, &LibraryLayout::OnCategoryViewInvalidated);

    this->Layout();
}

IWindowPtr LibraryLayout::GetFocus() {
    return this->focused ? this->focused : LayoutBase::GetFocus();
}

void LibraryLayout::Show() {
    LayoutBase::Show();
    this->categoryList->Requery();
}

void LibraryLayout::RequeryTrackList(ListWindow *view) {
    if (view == this->categoryList.get()) {
        DBID id = this->categoryList->GetSelectedId();
        if (id != -1) {
            this->trackList->Requery(this->categoryList->GetFieldName(), id);
        }
    }
}

void LibraryLayout::OnCategoryViewSelectionChanged(
    ListWindow *view, size_t newIndex, size_t oldIndex) {
    this->RequeryTrackList(view);
}

void LibraryLayout::OnCategoryViewInvalidated(
    ListWindow *view, size_t selectedIndex) {
    this->RequeryTrackList(view);
}

bool LibraryLayout::KeyPress(const std::string& key) {
    if (key == "ALT_1" || key == "M-1") {
        this->categoryList->SetFieldName(constants::Track::ARTIST_ID);
        return true;
    }
    else if (key == "ALT_2" || key == "M-2") {
        this->categoryList->SetFieldName(constants::Track::ALBUM_ID);
        return true;
    }
    else if (key == "ALT_3" || key == "M-3") {
        this->categoryList->SetFieldName(constants::Track::GENRE_ID);
        return true;
    }
    else if (key == "KEY_F(5)") {
        this->categoryList->Requery();
        return true;
    }
    else if (key == "CTL_DOWN") {
        this->focused = this->transportView;
        this->transportView->Focus();
    }
    else if (key == "CTL_UP") {
        if (this->focused) {
            this->focused->Blur();
            this->focused.reset();
        }
    }
    else if (key == " ") {
        /* copied from GlobalHotkeys. should probably be generalized
        at some point. */
        int state = this->transport.GetPlaybackState();
        if (state == Transport::PlaybackPaused) {
            this->transport.Resume();
        }
        else if (state == Transport::PlaybackPlaying) {
            this->transport.Pause();
        }
    }

    return LayoutBase::KeyPress(key);
}
