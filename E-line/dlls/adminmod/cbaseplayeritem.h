

typedef struct
{
        const char *pszName;
        int iId;
} AmmoInfo;

typedef struct
{
        int             iSlot;
        int             iPosition;
        const char      *pszAmmo1;      // ammo 1 type
        int             iMaxAmmo1;              // max ammo 1
        const char      *pszAmmo2;      // ammo 2 type
        int             iMaxAmmo2;              // max ammo 2
        const char      *pszName;
        int             iMaxClip;
        int             iId;
        int             iFlags;
        int             iWeight;// this value used to determine this weapon's importance in autoselection.
} ItemInfo;


class CBasePlayerItem : public CBaseAnimating
{
public:
        virtual void SetObjectCollisionBox( void );

        virtual int             Save( CSave &save );
        virtual int             Restore( CRestore &restore );

        static  TYPEDESCRIPTION m_SaveData[];

        virtual int AddToPlayer( CBasePlayer *pPlayer );        // return TRUE if the ite$
        virtual int AddDuplicate( CBasePlayerItem *pItem ) { return FALSE; }    // return
        void EXPORT DestroyItem( void );
        void EXPORT DefaultTouch( CBaseEntity *pOther );        // default weapon touch
        void EXPORT FallThink ( void );// when an item is first spawned, this think is ru$
        void EXPORT Materialize( void );// make a weapon visible and tangible
        void EXPORT AttemptToMaterialize( void );  // the weapon desires to become visibl$
        CBaseEntity* Respawn ( void );// copy a weapon
        void FallInit( void );
        void CheckRespawn( void );
        virtual int GetItemInfo(ItemInfo *p) { return 0; };     // returns 0 if struct no$
        virtual BOOL CanDeploy( void ) { return TRUE; };
        virtual BOOL CanDrop() { return TRUE; }
        virtual BOOL Deploy();                               
        virtual BOOL IsWeapon() { return FALSE; }

        virtual BOOL CanHolster( void ) { return TRUE; };// can this weapon be put away r$
        virtual void Holster( int skiplocal = 0 );
        virtual void UpdateItemInfo( void ) { return; };

        virtual void ItemPreFrame( void )       { return; }             // called each fr$
        virtual void ItemPostFrame( void ) { return; }          // called each frame by t$

        virtual void Drop( void );
        virtual void Kill( void );
        virtual void AttachToPlayer ( CBasePlayer *pPlayer );
       virtual int PrimaryAmmoIndex() { return -1; };
        virtual int SecondaryAmmoIndex() { return -1; };

        virtual int UpdateClientData( CBasePlayer *pPlayer ) { return 0; }
        virtual CBasePlayerItem *GetWeaponPtr (void) {return NULL; };
        virtual float GetMaxSpeed() { return 260; } // Override to specify different play$

        static ItemInfo ItemInfoArray[ MAX_WEAPONS ];
        static AmmoInfo AmmoInfoArray[ MAX_AMMO_SLOTS ];

        CBasePlayer     *m_pPlayer;
        CBasePlayerItem *m_pNext;
        int             m_iId;                                           

        virtual int iItemSlot( void ) { return 0; }                     // return 0 to MA$


        int                     iItemPosition( void ) { }
        const char      *pszAmmo1( void )       {}
        int                     iMaxAmmo1( void )       {}
        const char      *pszAmmo2( void )       {}
        int                     iMaxAmmo2( void )       {}
        const char      *pszName( void )        {}
        int                     iMaxClip( void )        {}
        int                     iWeight( void )         { }
        int                     iFlags( void )          {}
};

